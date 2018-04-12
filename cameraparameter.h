#ifndef CAMERAPARAMETER_H
#define CAMERAPARAMETER_H

#include <opencv2/opencv.hpp>
#include <QGenericMatrix>
#include <QVector4D>

class CameraParameter
{
public:
    CameraParameter();

    QMatrix3x3 qIntrinsic;
    QMatrix4x3 qExtrinsic;
    QVector4D qDistCoeffs;

    cv::Mat cvIntrinsic;
    cv::Mat cvExtrinsic;
    cv::Mat cvDistCoeffs;

    bool intrinsicDistReady;
    bool extrinsicReady;
    QString name;
    int devId;

    static bool changed;
    static cv::Size imgSize;
    static cv::Size boardSize;
    static float squareSize;
    static float imageWidthToHeight;
};

#endif // CAMERAPARAMETER_H
