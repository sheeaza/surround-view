#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdio.h>

#include <QObject>
#include <QMutex>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include "cameraparameter.h"

using namespace cv;
using namespace std;

class CameraCalibrator : public QObject
{
    Q_OBJECT

public:
    CameraCalibrator(CameraParameter &p, QObject *parent = 0);
    ~CameraCalibrator();

    void calibrateImage(Mat &distorImage, Mat &undistortImage);
    void initCalibMap();
    void cleanPoints();

    Mat intrinsic;
    Mat distCoeffs;
    double totalAvgErr;

signals:
    void isChessboardFound(bool found);
    void sendCalibResults(bool ok);

public slots:
    void findAndDrawChessboard(Mat *input);
    void calibrate();

private:
    inline double computeReprojectionErrors(const vector<vector<Point3f> > &objectPoints,
                                     const vector<vector<Point2f> > &imagePoints,
                                     const vector<Mat> &rvecs, const vector<Mat> &tvecs,
                                     const Mat &intrinsic, const Mat &distCoeffs,
                                     vector<float> &perViewErrors);
    Size boardSize;
    float squareSize;
    Size imgSize;

    bool useCalibrate;

    Mat chessboardImage;
    vector<vector<Point2f> > imagePoints;

    Mat map1, map2;
};

#endif // CALIBRATION_H
