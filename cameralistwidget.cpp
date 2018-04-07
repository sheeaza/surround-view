#include "cameralistwidget.h"
#include "ui_cameralistwidget.h"

#include <QDebug>
#include <QImage>
#include <QScrollBar>
#include <QLayout>
#include <QLineEdit>

CameraListWidget::CameraListWidget(Attribute attr, QVector<CameraParameter> &pv, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraListWidget),
    pvec(pv)
{
    ui->setupUi(this);

    for(QVector<CameraParameter>::iterator it = pvec.begin();
        it != pvec.end(); it++) {
        QLabel *imgLabel = new QLabel;

        QString s = it->name + ":\n";
        s += (it->intrinsicDistReady ? "calibrated" : "to be calibrated");
        QLabel *calibStatusLabel = new QLabel(s);
        camInfoVec.append(calibStatusLabel);

        QPushButton *calibButton = new QPushButton("calibrate");
        calibButton->setMinimumHeight(50);

        //grabe an image to display
        cv::VideoCapture cap(it->devId);
        if(cap.isOpened()) {
            cv::Mat cvImage;
            QImage img;
            cap >> cvImage;
            cv::cvtColor(cvImage, cvImage, CV_BGR2RGB);
            CalibrateWidget::matToQimage(cvImage, img);
            imgLabel->setPixmap(QPixmap::fromImage(
                                    img.scaled(150,150,Qt::KeepAspectRatio)));
        } else {
            calibButton->setEnabled(false);
        }

        QHBoxLayout *hlayout = new QHBoxLayout;
        hlayout->addWidget(imgLabel);
        hlayout->addWidget(calibStatusLabel);
        hlayout->addWidget(calibButton);
        ui->verticalLayout->addLayout(hlayout);

        connect(calibButton, &QPushButton::clicked,
                [=](){CalibrateWidget *calibw = new CalibrateWidget(*it, this);
            calibw->setAttribute(Qt::WA_DeleteOnClose);
            calibw->setWindowTitle(it->name);
            calibw->exec();});
    }
}

CameraListWidget::~CameraListWidget()
{
    delete ui;
}
