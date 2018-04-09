#ifndef SURROUNDVIEWDIALOG_H
#define SURROUNDVIEWDIALOG_H

#include <QDialog>

#include "v4l2capture.h"
#include "cameraparameter.h"
#include "gpurender.h"

namespace Ui {
class SurroundViewDialog;
}

class SurroundViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SurroundViewDialog(QVector<CameraParameter> &p,
                                QWidget *parent = 0);
    ~SurroundViewDialog();
public slots:
    void animate();

private:
    void initQParameters();

    Ui::SurroundViewDialog *ui;
    GpuRender *renderWidget;
    QVector<V4l2Capture *> v4l2Cap;
    QVector<CameraParameter> &camParas;
};

#endif // SURROUNDVIEWDIALOG_H
