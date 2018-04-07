#include "cameraparameter.h"

CameraParameter::CameraParameter()
{

}

CameraParameter::CameraParameter(const std::string &file, const std::string &camera)
{
    read(file, camera);
}

void CameraParameter::read(const std::string &file, const std::string &camera)
{
    cv::FileStorage fs(file, cv::FileStorage::READ);

    cv::FileNode n = fs[camera];

    cv::Mat cvIntrinsic;
    cv::Mat cvDistCoeffs;
    cv::Mat cvExtrinsic;

    n["intrinsic"] >> cvIntrinsic;
    n["distCoeffs"] >> cvDistCoeffs;
    n["extrinsic"] >> cvExtrinsic;

    cvIntrinsic.convertTo(cvIntrinsic, CV_32F);
    cvDistCoeffs.convertTo(cvDistCoeffs, CV_32F);
    cvExtrinsic.convertTo(cvExtrinsic, CV_32F);

    intrinsic = QMatrix3x3((float *)cvIntrinsic.data);
    distCoeffs = QVector4D(cvDistCoeffs.at<float>(0), cvDistCoeffs.at<float>(1),
                           cvDistCoeffs.at<float>(1), cvDistCoeffs.at<float>(2));
    extrinsic = QMatrix4x3((float*)cvExtrinsic.data);
}
