#ifndef CALIBRATEWIDGET_H
#define CALIBRATEWIDGET_H

#include <QDialog>
#include <QDebug>
#include <QTimer>
#include <QThread>

#include "cameraparameter.h"
#include "cameraCalibrator.h"

#include <opencv2/opencv.hpp>

namespace Ui {
class CalibrateWidget;
}

class CalibrateWidget : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrateWidget(CameraParameter &p, QWidget *parent = 0);
    ~CalibrateWidget();

    static void matToQimage(const Mat &matImage, QImage &qImage) {
        qImage = QImage((const unsigned char*)(matImage.data), matImage.cols,
                        matImage.rows, matImage.step, QImage::Format_RGB888);
    }

signals:
    void sendSrcImage(cv::Mat *);
    void startCalibration();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
    void foundChessboard(bool f);

private slots:
    void on_captureButton_clicked();
    void on_srcSaveButton_clicked();
    void on_dstSaveButton_clicked();
    void on_saveParaButton_clicked();
    void on_resetButton_clicked();

    void cameraTimeout();
    void receiveCalibResults(bool ok);

private:
    inline void updateFrameNums();

    Ui::CalibrateWidget *ui;

    CameraCalibrator *camCalib;
    QThread calibThread;

    CameraParameter &camPara;
    cv::VideoCapture capture;
    QTimer *captureTimer;
    cv::Mat srcImage, dstImage;
    int frames;
    bool showUndistort;
};

#endif // CALIBRATEWIDGET_H
