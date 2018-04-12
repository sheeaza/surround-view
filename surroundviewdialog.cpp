#include "surroundviewdialog.h"
#include "ui_surroundviewdialog.h"
#include "aspectsinglelayout.h"

SurroundViewDialog::SurroundViewDialog(QVector<CameraParameter> &p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SurroundViewDialog),
    camParas(p)
{
    ui->setupUi(this);

    initQParameters();
    ui->renderWidget->setCameraPara(camParas);

    AspectSingleLayout *aspLayout =
            new AspectSingleLayout(NULL, camParas.at(0).imageWidthToHeight);
    ui->gridLayout->addLayout(aspLayout, 0, 0);
    aspLayout->setContentsMargins(0, 0, 0, 0);
    aspLayout->addWidget(ui->renderWidget);
    aspLayout->setAlignment(ui->renderWidget, Qt::AlignCenter);

    V4l2Capture::ImgFormat fmt;
    fmt.height = camParas.at(0).imgSize.height;
    fmt.width = camParas.at(0).imgSize.width;
    fmt.pixFmt = V4l2Capture::UYVY;
    for (auto &item : camParas) {
        V4l2Capture *cap = new V4l2Capture(item.devId, fmt);
        cap->startCapturing();
        v4l2Cap.append(cap);
    }
    ui->renderWidget->allocate_buffer(v4l2Cap.size());

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SurroundViewDialog::animate);
    timer->start(33);
}

SurroundViewDialog::~SurroundViewDialog()
{
    delete ui;
    foreach (auto &item, v4l2Cap) {
        delete item;
    }
}

void SurroundViewDialog::animate()
{
    QVector<V4l2Capture::CapBuffers *> buf(v4l2Cap.size());

    ui->renderWidget->enablePaint();

    for(int i = 0; i < v4l2Cap.size(); i++) {
        v4l2Cap.at(i)->getFrame(&buf[i]);
    }
    ui->renderWidget->setBuf(buf);

    ui->renderWidget->update();

    foreach (auto &item, v4l2Cap) {
        item->doneFrame();
    }
}

void SurroundViewDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if(isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

void SurroundViewDialog::initQParameters()
{
    cv::Mat cvIntrinsicTmp, cvDistTmp, cvExtrinsicTmp;

    for(CameraParameter &item : camParas) {
        item.cvIntrinsic.convertTo(cvIntrinsicTmp, CV_32F);
        item.cvDistCoeffs.convertTo(cvDistTmp, CV_32F);
        item.cvExtrinsic.convertTo(cvExtrinsicTmp, CV_32F);

        item.qIntrinsic = QMatrix3x3((float*)cvIntrinsicTmp.data);
        item.qDistCoeffs = QVector4D(cvDistTmp.at<float>(0), cvDistTmp.at<float>(1),
                                     cvDistTmp.at<float>(2), cvDistTmp.at<float>(3));
        item.qExtrinsic = QMatrix4x3((float*)cvExtrinsicTmp.data);
    }
}
