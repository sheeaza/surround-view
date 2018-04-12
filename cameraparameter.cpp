#include "cameraparameter.h"

cv::Size CameraParameter::imgSize(720, 480);
cv::Size CameraParameter::boardSize(8, 5);
float CameraParameter::squareSize(30);
bool CameraParameter::changed(false);
float CameraParameter::imageWidthToHeight(1.5);

CameraParameter::CameraParameter() :
    intrinsicDistReady(false),
    extrinsicReady(false),
    devId(-1)
{

}
