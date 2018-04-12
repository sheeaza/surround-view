#include "cameralistwidget.h"
#include "extrinsicdialog.h"
#include "aspectsinglelayout.h"
#include "ui_cameralistwidget.h"

#include <functional>

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
        imgLabel->setFrameShape(QLabel::Box);
        imgLabel->setFrameShadow(QLabel::Raised);
        imgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        imgLabel->setMinimumSize(270, 180);
        imgLabel->setScaledContents(true);

        AspectSingleLayout *aspLayout =
                new AspectSingleLayout(NULL, it->imageWidthToHeight);
        aspLayout->setContentsMargins(0, 0, 0, 0);
        aspLayout->addWidget(imgLabel);
        aspLayout->setAlignment(imgLabel, Qt::AlignCenter);

        QPushButton *calibButton = new QPushButton("calibrate");
        calibButton->setMinimumSize(90, 60);
        calibButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

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
            imgLabel->setPixmap(QPixmap::fromImage(img));
        } else {
            calibButton->setEnabled(false);
        }

        QLabel *calibStatusLabel = new QLabel;

        std::function<void(void)> lfp;
        bool ready(0);
        if(attr == Attribute::intrinsic) {
            ready = it->intrinsicDistReady;

            lfp = [=](){CalibrateWidget *calibw = new CalibrateWidget(*it, this);
                        calibw->setAttribute(Qt::WA_DeleteOnClose);
                        calibw->setWindowTitle(it->name);
                        calibw->exec();
                        setCalibrateStatus(calibStatusLabel,
                                           it, it->intrinsicDistReady);};
        } else if (attr == Attribute::extrinsic) {
            ready = it->extrinsicReady;

            lfp = [=](){cv::Mat img;
                        cvImage.copyTo(img);
                        ExtrinsicDialog *exDialog = new ExtrinsicDialog(img, *it, this);
                        exDialog->setAttribute(Qt::WA_DeleteOnClose);
                        exDialog->setWindowTitle(it->name);
                        exDialog->exec();
                        setCalibrateStatus(calibStatusLabel,
                                           it, it->extrinsicReady);};

            if(!it->intrinsicDistReady) {
                calibButton->setEnabled(false);
            }
        }
        setCalibrateStatus(calibStatusLabel, it, ready);

        QHBoxLayout *hlayout = new QHBoxLayout;
        hlayout->addLayout(aspLayout, 5);
        hlayout->addWidget(calibStatusLabel, 1);
        hlayout->addWidget(calibButton, 1);
        ui->verticalLayout->addLayout(hlayout);

        connect(calibButton, &QPushButton::clicked, lfp);
    }
}

CameraListWidget::~CameraListWidget()
{
    delete ui;
}

void CameraListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if(isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

void CameraListWidget::setCalibrateStatus(QLabel *label, CameraParameter *p, bool ready)
{
    QString s = p->name + ":\n";
    s += (ready ? "calibrated" : "to be calibrated");
    label->setText(s);
}
