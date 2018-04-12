#ifndef EXTRINSICDIALOG_H
#define EXTRINSICDIALOG_H

#include <QDialog>

#include "cameraparameter.h"

#include <opencv2/opencv.hpp>

namespace Ui {
class ExtrinsicDialog;
}

class ExtrinsicDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExtrinsicDialog(cv::Mat &image,
                             CameraParameter &p, QWidget *parent = 0);
    ~ExtrinsicDialog();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
    void findChessboardCorners(const QPoint&, const QPoint&);
    void on_resetButton_clicked();

private:
    void makeExtrinsic();

    Ui::ExtrinsicDialog *ui;
    CameraParameter &camPara;
    cv::Mat srcImage;
    std::vector<cv::Point2f> subCorners;
};

#endif // EXTRINSICDIALOG_H
