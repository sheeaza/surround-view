#include "cameraCalibrator.h"
#include <QThread>
#include <QDebug>

#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <iostream>
#include <fstream>

void myInitUndistortRectifyMap( InputArray K, InputArray D, InputArray R, InputArray P,
    const cv::Size& size, int m1type, OutputArray map1, OutputArray map2 );

CameraCalibrator::CameraCalibrator(QObject *parent)
    : QObject(parent)
{

}

CameraCalibrator::~CameraCalibrator()
{
    closeCamera();
}

void CameraCalibrator::loadSetting(const string &filename)
{
    FileStorage fs(filename, FileStorage::READ);

    if (!fs.isOpened()) {
        qWarning() << "Could not open the configuration file: \"" << filename.c_str() << "\"";
        exit(1);
    }

    fs["Settings"]["cameraID"] >> cameraID;
    fs["Settings"]["BoardSize_Width"] >> boardSize.width;
    fs["Settings"]["BoardSize_Height"] >> boardSize.height;
    fs["Settings"]["Square_Size"] >> squareSize;
    fs["Settings"]["Write_outputFileName"] >> outputFileName;
    fs["Settings"]["useCalibrate"] >> useCalibrate;
    printf("%d\n", cameraID);
}

void CameraCalibrator::openCamera()
{
    inputCapture.open(cameraID);

    inputCapture.set(CAP_PROP_FRAME_WIDTH, 720);
    inputCapture.set(CAP_PROP_FRAME_HEIGHT, 480);

    if (!inputCapture.isOpened()) {
        qWarning() << "Cannot open camera: " << cameraID << endl;
        exit(1);
    }

    if (useCalibrate) {
        FileStorage pfs(outputFileName, FileStorage::READ);
        string id = "_" + to_string(cameraID);
        pfs["intrinsic_mat" + id] >> cameraMatrix;
        pfs["dostCoeffs" + id] >> distCoeffs;

        Mat viewtemp;
        inputCapture >> viewtemp;
        Size image_size = viewtemp.size();

        fisheye::initUndistortRectifyMap(cameraMatrix, distCoeffs, Matx33d::eye(),
                                         cameraMatrix, image_size, CV_32FC1,
                                         map1, map2);

        emit sendCalibResults(1, 0, 0);
    }
}

void CameraCalibrator::closeCamera()
{
    inputCapture.release();
}

void CameraCalibrator::getCvImage(Mat &image)
{
    mutex.lock();
    inputCapture >> image;
    mutex.unlock();
}

void CameraCalibrator::getChessboardImage(Mat &image)
{
    chessboardImage.copyTo(image);
}

void CameraCalibrator::calibrateImage(Mat &distorImage, Mat &undistortImage)
{
    cv::remap(distorImage, undistortImage, map1, map2, INTER_LINEAR);
}

void CameraCalibrator::findAndDrawChessboard()
{
    Mat view;

    mutex.lock();
    inputCapture >> view;
    mutex.unlock();

    vector<Point2f> pointBuf;

    bool found = findChessboardCorners(view, boardSize, pointBuf);
    if (found) {
        Mat viewGray;
        cvtColor(view, viewGray, COLOR_BGR2GRAY);
        cornerSubPix(viewGray, pointBuf, Size(7,7), Size(-1,-1),
                     TermCriteria(TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1));
        imagePoints.push_back(pointBuf);
        drawChessboardCorners(view, boardSize, Mat(pointBuf), found);
        view.copyTo(chessboardImage);
    }
    emit sendChessboardImage(found);
}

void CameraCalibrator::calibrate()
{
    Mat _rvecs, _tvecs;
    vector<Mat> rvecs, tvecs;

    double rms;

    cameraMatrix = Mat::eye(3, 3, CV_64F);
    distCoeffs = Mat::zeros(4, 1, CV_64F);

    vector<vector<Point3f> > objectPoints(1);

    objectPoints[0].clear();
    for( int i = 0; i < boardSize.height; ++i )
        for( int j = 0; j < boardSize.width; ++j )
            objectPoints[0].push_back(Point3f(j*squareSize, i*squareSize, 0));
    objectPoints.resize(imagePoints.size(), objectPoints[0]);

    Mat viewtemp;
    mutex.lock();
    inputCapture >> viewtemp;
    mutex.unlock();
    Size image_size = viewtemp.size();
    int flags = 0;
    flags |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
    flags |= cv::fisheye::CALIB_CHECK_COND;
    flags |= cv::fisheye::CALIB_FIX_SKEW;
    rms = fisheye::calibrate(objectPoints, imagePoints, image_size,
                             cameraMatrix, distCoeffs, _rvecs, _tvecs,
                             flags);
    rvecs.reserve(_rvecs.rows);
    tvecs.reserve(_tvecs.rows);
    for(int i = 0; i < int(objectPoints.size()); i++){
        rvecs.push_back(_rvecs.row(i));
        tvecs.push_back(_tvecs.row(i));
    }
    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

    if (ok) {
        vector<float> reprojErrs;
        totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                                                       rvecs, tvecs, cameraMatrix,
                                                       distCoeffs, reprojErrs);
        fisheye::initUndistortRectifyMap(cameraMatrix, distCoeffs, Matx33d::eye(),
                                         cameraMatrix, image_size, CV_16SC2,
                                         map1, map2);
    }
    emit sendCalibResults(ok, totalAvgErr, rms);
}

void CameraCalibrator::saveParams()
{
    FileStorage fs(outputFileName, FileStorage::WRITE);

    string id = "_" + to_string(cameraID);
    fs << "intrinsic_mat" + id << cameraMatrix;
    fs << "dostCoeffs" + id << distCoeffs;
    fs << "totalAvgErr" + id << totalAvgErr;
}

double CameraCalibrator::computeReprojectionErrors(const vector<vector<Point3f> > &objectPoints, const vector<vector<Point2f> > &imagePoints, const vector<Mat> &rvecs, const vector<Mat> &tvecs, const Mat &cameraMatrix, const Mat &distCoeffs, vector<float> &perViewErrors)
{
    vector<Point2f> imagePoints2;
    size_t totalPoints = 0;
    double totalErr = 0, err;
    perViewErrors.resize(objectPoints.size());

    for(size_t i = 0; i < objectPoints.size(); ++i )
    {
        fisheye::projectPoints(objectPoints[i], imagePoints2, rvecs[i], tvecs[i], cameraMatrix,
                               distCoeffs);

        err = norm(imagePoints[i], imagePoints2, NORM_L2);

        size_t n = objectPoints[i].size();
        perViewErrors[i] = (float) std::sqrt(err*err/n);
        totalErr        += err*err;
        totalPoints     += n;
    }

    return std::sqrt(totalErr/totalPoints);
}


