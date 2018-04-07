#ifndef CAMERAPARAMETER_H
#define CAMERAPARAMETER_H

#include <opencv2/opencv.hpp>
#include <QGenericMatrix>
#include <QVector4D>

class CameraParameter
{
public:
    CameraParameter();
    CameraParameter(const std::string &file, const std::string &camera);
    void read(const std::string &file, const std::string &camera);

    QMatrix3x3 intrinsic;
    QVector4D distCoeffs;
    QMatrix4x3 extrinsic;
};

#endif // CAMERAPARAMETER_H
