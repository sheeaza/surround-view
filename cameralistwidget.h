#ifndef CAMERALIST_H
#define CAMERALIST_H

#include <QDialog>
#include <QLabel>

#include "calibratewidget.h"
#include "cameraparameter.h"

namespace Ui {
class CameraListWidget;
}

class CameraListWidget : public QDialog
{
    Q_OBJECT

public:
    enum Attribute {
        intrinsic = 0,
        extrinsic,
    };

    explicit CameraListWidget(Attribute attr,
                              QVector<CameraParameter> &pv,
                              QWidget *parent = 0);
    ~CameraListWidget();

private slots:

private:
    Ui::CameraListWidget *ui;
    QVector<CameraParameter> &pvec;
    QVector<QLabel *> camInfoVec;
};

#endif // CAMERALIST_H
