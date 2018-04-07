#include "cameraCalibrator.h"
#include <QThread>
#include <QDebug>

#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <iostream>
#include <fstream>

#include "cameraparameter.h"

CameraCalibrator::CameraCalibrator(QObject *parent)
    : QObject(parent)
{
    boardSize = CameraParameter::boardSize;
    squareSize = CameraParameter::squareSize;
    imgSize = CameraParameter::imgSize;
}

CameraCalibrator::~CameraCalibrator()
{
}

void CameraCalibrator::calibrateImage(Mat &distorImage, Mat &undistortImage)
{
    cv::remap(distorImage, undistortImage, map1, map2, INTER_LINEAR);
}

void CameraCalibrator::initCalibMap()
{
    fisheye::initUndistortRectifyMap(intrinsic, distCoeffs, Matx33d::eye(),
                                     intrinsic, imgSize, CV_16SC2,
                                     map1, map2);
}

void CameraCalibrator::findAndDrawChessboard(cv::Mat *input)
{
    vector<Point2f> pointBuf;

    bool found = findChessboardCorners((*input), boardSize, pointBuf);
    if (found) {
        Mat viewGray;
        cvtColor((*input), viewGray, COLOR_RGB2GRAY);
        cornerSubPix(viewGray, pointBuf, Size(7,7), Size(-1,-1),
                     TermCriteria(TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1));
        imagePoints.push_back(pointBuf);
        drawChessboardCorners((*input), boardSize, Mat(pointBuf), found);
    }
    emit isChessboardFound(found);
}

void CameraCalibrator::calibrate()
{
    Mat _rvecs, _tvecs;
    vector<Mat> rvecs, tvecs;

    intrinsic = Mat::eye(3, 3, CV_64F);
    distCoeffs = Mat::zeros(4, 1, CV_64F);

    vector<vector<Point3f> > objectPoints(1);

    objectPoints[0].clear();
    for( int i = 0; i < boardSize.height; ++i )
        for( int j = 0; j < boardSize.width; ++j )
            objectPoints[0].push_back(Point3f(j*squareSize, i*squareSize, 0));
    objectPoints.resize(imagePoints.size(), objectPoints[0]);

    int flags = 0;
    flags |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
    flags |= cv::fisheye::CALIB_CHECK_COND;
    flags |= cv::fisheye::CALIB_FIX_SKEW;
    fisheye::calibrate(objectPoints, imagePoints, imgSize,
                             intrinsic, distCoeffs, _rvecs, _tvecs,
                             flags);
    rvecs.reserve(_rvecs.rows);
    tvecs.reserve(_tvecs.rows);
    for(int i = 0; i < int(objectPoints.size()); i++){
        rvecs.push_back(_rvecs.row(i));
        tvecs.push_back(_tvecs.row(i));
    }

    bool ok = checkRange(intrinsic) && checkRange(distCoeffs);
    if (ok) {
        vector<float> reprojErrs;
        totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                                                       rvecs, tvecs, intrinsic,
                                                       distCoeffs, reprojErrs);
        initCalibMap();
    }
    emit sendCalibResults(ok);
}

void CameraCalibrator::cleanPoints()
{
    imagePoints.clear();
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
