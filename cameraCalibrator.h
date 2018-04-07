#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdio.h>

#include <QObject>
#include <QMutex>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

using namespace cv;
using namespace std;

class CameraCalibrator : public QObject
{
    Q_OBJECT

public:
    CameraCalibrator(QObject *parent = 0);
    ~CameraCalibrator();

    void loadSetting(const string &filename);
    void openCamera();
    void closeCamera();

    void getCvImage(Mat &image);
    void getChessboardImage(Mat &image);
    void calibrateImage(Mat &distorImage, Mat &undistortImage);

signals:
    void sendChessboardImage(bool found);
    void sendCalibResults(bool ok, double totalAvgErr, double reproErr);

public slots:
    void findAndDrawChessboard();
    void calibrate();
    void saveParams();

private:
    double computeReprojectionErrors(const vector<vector<Point3f> > &objectPoints,
                                     const vector<vector<Point2f> > &imagePoints,
                                     const vector<Mat> &rvecs, const vector<Mat> &tvecs,
                                     const Mat &cameraMatrix, const Mat &distCoeffs,
                                     vector<float> &perViewErrors);

    QMutex mutex;
    VideoCapture inputCapture;
    int cameraID;
    Size boardSize;
    float squareSize;
    string outputFileName;
    bool useCalibrate;

    Mat chessboardImage;
    vector<vector<Point2f> > imagePoints;

    Mat cameraMatrix;
    Mat distCoeffs;
    double totalAvgErr;
    Mat map1, map2;
};

#endif // CALIBRATION_H
