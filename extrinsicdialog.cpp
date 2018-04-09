#include "extrinsicdialog.h"
#include "ui_extrinsicdialog.h"

#include "calibratewidget.h"

#include <QRect>
#include <QPoint>
#include <QPainter>

ExtrinsicDialog::ExtrinsicDialog(cv::Mat &image,
                                 CameraParameter &p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtrinsicDialog),
    camPara(p),
    found(0)
{
    ui->setupUi(this);

    ui->imageLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);

    image.copyTo(srcImage);

    QImage qImg;
    CalibrateWidget::matToQimage(srcImage, qImg);
    QSize sz = ui->imageLabel->size();
    QImage scaledImg(qImg.scaled(sz, Qt::KeepAspectRatio));
    scaledSize = scaledImg.size();
    pix = QPixmap::fromImage(scaledImg);
    ui->imageLabel->setPixmap(pix);
    scaleFactor = (float)(qImg.size().width()) / scaledSize.width();

    connect(ui->imageLabel, &ExtrinsicLabel::mouseReleased,
            this, &ExtrinsicDialog::findChessboardCorners);
}

ExtrinsicDialog::~ExtrinsicDialog()
{
    delete ui;
}

void ExtrinsicDialog::findChessboardCorners(const QPoint &origin, const QPoint &end)
{
    if(found)
        return;

    QPoint endBoundp = end;
    endBoundp.rx() = qBound(0, endBoundp.x(), scaledSize.width()-1);
    endBoundp.ry() = qBound(0, endBoundp.y(), scaledSize.height()-1);

    QRect qRectMask = QRect(origin * scaleFactor, endBoundp * scaleFactor).normalized();

    QPoint qorigin = qRectMask.topLeft();
    QSize qsize = qRectMask.size();

    cv::Point2f biasMain(qorigin.x(), qorigin.y());
    cv::Size mainRoiSize(qsize.width(), qsize.height());
    cv::Rect cvRectMask(biasMain, mainRoiSize);

    cv::Mat cvSrcImg;
    srcImage.copyTo(cvSrcImg);
    cv::Mat mainRoiImg = cvSrcImg(cvRectMask);

    cv::Size halfRoiSize(mainRoiSize.width / 2, mainRoiSize.height);
    cv::Point2f biasRight(mainRoiSize.width / 2, 0);
    cv::Rect cvRectLeft(cv::Point(0,0), halfRoiSize);
    cv::Rect cvRectRight(biasRight, halfRoiSize);

    cv::Mat leftRoiImg = mainRoiImg(cvRectLeft);
    cv::Mat rightRoiImg = mainRoiImg(cvRectRight);

    vector<cv::Mat> subChessBoardImgs;
    subChessBoardImgs.push_back(leftRoiImg);
    subChessBoardImgs.push_back(rightRoiImg);

    vector<cv::Point2f> originPoints;
    originPoints.push_back(cv::Point(0.0, 0.0));
    originPoints.push_back(cv::Point(halfRoiSize.width, 0.0));

    for(size_t i = 0; i < subChessBoardImgs.size(); ++i) {
        vector<cv::Point2f> corners;
        bool cfound;
        cv::Point2f far, near;
        cfound = cv::findChessboardCorners(subChessBoardImgs[i], cv::Size(3, 3), corners);
        if(!cfound) {
            qInfo() << "corners not found!";
            subCorners.clear();
            return;
        }
        cv::Mat grayImage;
        cv::cvtColor(subChessBoardImgs[i], grayImage, cv::COLOR_BGR2GRAY);
        cv::cornerSubPix(grayImage, corners, cv::Size(5, 5),
                         cv::Size(-1, -1),
                         cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.1));

        far = corners.at(0);
        near = corners.at(0);
        float resFar, resNear;
        resFar = resNear = cv::norm(far - originPoints[i]);
        for(size_t j = 1; j < corners.size(); ++j) {
            float res = cv::norm(corners[j] - originPoints[i]);
            if(res < resNear) {
                near = corners[j];
                resNear = res;
            } else if(res > resFar) {
                far = corners[j];
                resFar = res;
            }
        }
        subCorners.push_back(far);
        subCorners.push_back(near);
    }

    found = true;

    for(size_t i = 2; i < subCorners.size(); ++i) {
        subCorners[i] += biasRight;
    }

    QPainter painter(&pix);

    for(size_t i = 0; i < subCorners.size(); ++i) {
        QPointF pos;
        subCorners[i] += biasMain;
//        cv::circle(cvSrcImg, subCorners[i], 1, cv::Scalar(140, 155, 190), 1);
        pos = QPointF(subCorners[i].x / scaleFactor,
                       subCorners[i].y / scaleFactor);
        painter.drawEllipse(pos, 5, 5);
    }
    ui->imageLabel->setPixmap(pix);
    makeExtrinsic();
}

void ExtrinsicDialog::makeExtrinsic()
{
    vector<cv::Point3f> objPoints;

    /* to be changed */
    if(camPara.name == "front") {
        objPoints.push_back(cv::Point3f(-7.0, 12.0, 0.0));
        objPoints.push_back(cv::Point3f(-11.0, 16.0, 0.0));
        objPoints.push_back(cv::Point3f(7.0, 12.0, 0.0));
        objPoints.push_back(cv::Point3f(11.0, 16.0, 0.0));
    } else if(camPara.name == "left") {
        objPoints.push_back(cv::Point3f(-7.0, -12.0, 0.0));
        objPoints.push_back(cv::Point3f(-11.0, -16.0, 0.0));
        objPoints.push_back(cv::Point3f(-7.0, 12.0, 0.0));
        objPoints.push_back(cv::Point3f(-11.0, 16.0, 0.0));
    } else if(camPara.name == "back") {
        objPoints.push_back(cv::Point3f(7.0, -12.0, 0.0));
        objPoints.push_back(cv::Point3f(11.0, -16.0, 0.0));
        objPoints.push_back(cv::Point3f(-7.0, -12.0, 0.0));
        objPoints.push_back(cv::Point3f(-11.0, -16.0, 0.0));
    } else {
        objPoints.push_back(cv::Point3f(7.0, 12.0, 0.0));
        objPoints.push_back(cv::Point3f(11.0, 16.0, 0.0));
        objPoints.push_back(cv::Point3f(7.0, -12.0, 0.0));
        objPoints.push_back(cv::Point3f(11.0, -16.0, 0.0));
    }

    float alpha = 1.0/36;
//    cv::Point3f t3(0.0, 0.145, 0.0);
    cv::Point3f t3(0.0, 0.0, 0.0);
    for(uint i = 0; i < objPoints.size(); i++) {
        objPoints[i] = (objPoints[i] - t3)*alpha;
    }

//    std::cout << subCorners << std::endl;

//    cv::Mat intrinsicTmp;
//    cv::Mat scale = cv::Mat::eye(3, 3, CV_64F);
//    scale.at<double>(0, 0) = camPara.imgSize.width;
//    scale.at<double>(1, 1) = camPara.imgSize.height;
//    camPara.cvIntrinsic.copyTo(intrinsicTmp);
//    intrinsicTmp = scale * intrinsicTmp;
//    std::cout << intrinsicTmp << std::endl;

//    std::vector<cv::Point2f> undistps;
//    cv::fisheye::undistortPoints(subCorners, undistps,
//                                 camPara.cvIntrinsic, camPara.cvDistCoeffs);

//    std::cout << undistps << std::endl;

    for(uint i = 0; i < subCorners.size(); ++i) {
        subCorners[i].x = subCorners[i].x / (camPara.imgSize.width - 1);
        subCorners[i].y = subCorners[i].y / (camPara.imgSize.height - 1);
    }

    cv::Mat rvec, tvec, rmat, extrinsic;
    cv::solvePnP(objPoints, subCorners, camPara.cvIntrinsic,
                 camPara.cvDistCoeffs, rvec, tvec);
//    cv::solvePnP(objPoints, undistps, camPara.cvIntrinsic,
//                 cv::Mat(), rvec, tvec);
    cv::Rodrigues(rvec, rmat);
    cv::hconcat(rmat, tvec, extrinsic);
    extrinsic.copyTo(camPara.cvExtrinsic);

    camPara.extrinsicReady = true;
    CameraParameter::changed = true;
}
