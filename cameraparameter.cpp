#include "cameraparameter.h"

cv::Size CameraParameter::imgSize(0,0);
cv::Size CameraParameter::boardSize(0, 0);
float CameraParameter::squareSize(0);
bool CameraParameter::changed(false);

CameraParameter::CameraParameter() :
    intrinsicDistReady(false),
    extrinsicReady(false),
    devId(-1)
{

}
