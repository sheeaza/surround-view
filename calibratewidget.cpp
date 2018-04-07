#include "calibratewidget.h"
#include "ui_calibratewidget.h"

CalibrateWidget::CalibrateWidget(CameraParameter &p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalibrateWidget),
    camPara(p),
    frames(0),
    showUndistort(0)
{
    ui->setupUi(this);

    if (!capture.open(camPara.devId)) {
        qInfo() << "can not open " << camPara.name << "camera";
        exit(-1);
    }
    capture.set(CAP_PROP_FRAME_WIDTH, camPara.imgSize.width);
    capture.set(CAP_PROP_FRAME_HEIGHT, camPara.imgSize.height);

    camCalib = new CameraCalibrator;

    captureTimer = new QTimer(this);

    connect(captureTimer, &QTimer::timeout,
            this, &CalibrateWidget::cameraTimeout);

    connect(this, &CalibrateWidget::sendSrcImage,
            camCalib, &CameraCalibrator::findAndDrawChessboard);
    connect(camCalib, &CameraCalibrator::isChessboardFound,
            this, &CalibrateWidget::foundChessboard);
    connect(ui->calibrateButton, &QPushButton::clicked,
            camCalib, &CameraCalibrator::calibrate);
    connect(camCalib, &CameraCalibrator::sendCalibResults,
            this, &CalibrateWidget::receiveCalibResults);

    if(camPara.intrinsicDistReady) {
        showUndistort = 1;
        ui->resetButton->setEnabled(true);

        camPara.cvIntrinsic.copyTo(camCalib->intrinsic);
        camPara.cvDistCoeffs.copyTo(camCalib->distCoeffs);
        camCalib->initCalibMap();
    } else {
        ui->captureButton->setEnabled(true);
    }

    camCalib->moveToThread(&calibThread);
    connect(&calibThread, &QThread::finished, camCalib, &CameraCalibrator::deleteLater);
    calibThread.start();
    captureTimer->start(30);
}

CalibrateWidget::~CalibrateWidget()
{
    delete ui;
    calibThread.quit();
}

void CalibrateWidget::on_captureButton_clicked()
{
    if(srcImage.empty()) {
        return;
    }
    srcImage.copyTo(dstImage);
    emit sendSrcImage(&dstImage);

    ui->captureButton->setEnabled(false);
}

void CalibrateWidget::foundChessboard(bool f)
{
    ui->dstImageLabel->clear();
    ui->captureButton->setEnabled(true);

    if(f) {
        QImage i;
        matToQimage(dstImage, i);
        QSize sz= ui->dstImageLabel->size();
        ui->dstImageLabel->setPixmap(
                    QPixmap::fromImage(i.scaled(sz, Qt::KeepAspectRatio)));
        frames++;
        updateFrameNums();

        if(!ui->calibrateButton->isEnabled())
            ui->calibrateButton->setEnabled(true);
        if(!ui->resetButton->isEnabled())
            ui->resetButton->setEnabled(true);
    }
}

void CalibrateWidget::receiveCalibResults(bool ok)
{
    ui->resultLine->clear();

    if(ok) {
        ui->calibrateButton->setEnabled(false);
        ui->captureButton->setEnabled(false);
        ui->saveParaButton->setEnabled(true);

        showUndistort = true;
        ui->resultLine->setText(QString("avg error: %1").arg(camCalib->totalAvgErr));
    } else {
        ui->resultLine->setText(QString("calibration Failed!"));
    }
    ui->resetButton->setEnabled(true);
}

void CalibrateWidget::on_saveParaButton_clicked()
{
    ui->saveParaButton->setEnabled(false);
    ui->resetButton->setEnabled(true);

    camCalib->intrinsic.copyTo(camPara.cvIntrinsic);
    camCalib->distCoeffs.copyTo(camPara.cvDistCoeffs);
    camPara.intrinsicDistReady = true;
    camPara.changed = true;
}

void CalibrateWidget::on_resetButton_clicked()
{
    ui->resetButton->setEnabled(false);

    ui->captureButton->setEnabled(true);
    ui->saveParaButton->setEnabled(false);
    ui->calibrateButton->setEnabled(false);

    ui->dstImageLabel->clear();
    ui->resultLine->clear();
    frames = 0;
    updateFrameNums();
    camCalib->cleanPoints();
    showUndistort = false;

    if(camPara.intrinsicDistReady) {
        camPara.changed = true;
        camPara.intrinsicDistReady = false;
    }
}

void CalibrateWidget::updateFrameNums()
{
    ui->frameLine->setText(QString("frames: %1").arg(frames));
}


void CalibrateWidget::on_srcSaveButton_clicked()
{
    if(srcImage.empty())
        return;
    cv::imwrite("./images/src_1.jpg", srcImage);
}

void CalibrateWidget::on_dstSaveButton_clicked()
{
    if(dstImage.empty())
        return;
    cv::imwrite("./images/dst_1.jpg", dstImage);
}

void CalibrateWidget::cameraTimeout()
{
    capture >> srcImage;
    cvtColor(srcImage, srcImage, COLOR_BGR2RGB);

    QImage src;
    matToQimage(srcImage, src);
    QSize ssz = ui->srcImageLabel->size();
    ui->srcImageLabel->setPixmap(QPixmap::fromImage(src.scaled(ssz, Qt::KeepAspectRatio)));

    if(showUndistort) {
        camCalib->calibrateImage(srcImage, dstImage);
        QImage dst;
        matToQimage(dstImage, dst);
        QSize dsz = ui->dstImageLabel->size();
        ui->dstImageLabel->setPixmap(QPixmap::fromImage(dst.scaled(dsz, Qt::KeepAspectRatio)));
    }
}
