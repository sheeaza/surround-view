#include "cameralistwidget.h"
#include "ui_cameralistwidget.h"

#include <functional>

#include <QDebug>
#include <QImage>
#include <QScrollBar>
#include <QLayout>
#include <QLineEdit>

#include "extrinsicdialog.h"

CameraListWidget::CameraListWidget(Attribute attr, QVector<CameraParameter> &pv, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraListWidget),
    pvec(pv)
{
    ui->setupUi(this);

    for(QVector<CameraParameter>::iterator it = pvec.begin();
        it != pvec.end(); it++) {
        QLabel *imgLabel = new QLabel;

        QPushButton *calibButton = new QPushButton("calibrate");
        calibButton->setMinimumHeight(50);

        //grabe an image to display
        cv::VideoCapture cap(it->devId);
        cap.set(CAP_PROP_FRAME_WIDTH, it->imgSize.width);
        cap.set(CAP_PROP_FRAME_HEIGHT, it->imgSize.height);
        cv::Mat cvImage;
        if(cap.isOpened()) {
            QImage img;
            cap >> cvImage;
            cv::cvtColor(cvImage, cvImage, CV_BGR2RGB);
            CalibrateWidget::matToQimage(cvImage, img);
            imgLabel->setPixmap(QPixmap::fromImage(
                                    img.scaled(150,150,Qt::KeepAspectRatio)));
        } else {
            calibButton->setEnabled(false);
        }

        std::function<void(void)> lfp;
        bool ready(0);
        if(attr == Attribute::intrinsic) {
            ready = it->intrinsicDistReady;

            lfp = [=](){CalibrateWidget *calibw = new CalibrateWidget(*it, this);
                        calibw->setAttribute(Qt::WA_DeleteOnClose);
                        calibw->setWindowTitle(it->name);
                        calibw->exec();};
        } else if (attr == Attribute::extrinsic) {
            ready = it->extrinsicReady;

            lfp = [=](){cv::Mat img;
                        cvImage.copyTo(img);
                        ExtrinsicDialog *exDialog = new ExtrinsicDialog(img, *it, this);
                        exDialog->setAttribute(Qt::WA_DeleteOnClose);
                        exDialog->setWindowTitle(it->name);
                        exDialog->exec();};

            if(!it->intrinsicDistReady) {
                calibButton->setEnabled(false);
            }
        }

        QString s = it->name + ":\n";
        s += (ready ? "calibrated" : "to be calibrated");
        QLabel *calibStatusLabel = new QLabel(s);
        camInfoVec.append(calibStatusLabel);

        QHBoxLayout *hlayout = new QHBoxLayout;
        hlayout->addWidget(imgLabel);
        hlayout->addWidget(calibStatusLabel);
        hlayout->addWidget(calibButton);
        ui->verticalLayout->addLayout(hlayout);

        connect(calibButton, &QPushButton::clicked, lfp);
    }
}

CameraListWidget::~CameraListWidget()
{
    delete ui;
}
