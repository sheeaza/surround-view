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

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

private slots:

private:
    void setCalibrateStatus(QLabel *label, CameraParameter *p,
                            bool ready);

    Ui::CameraListWidget *ui;
    QVector<CameraParameter> &pvec;
};

#endif // CAMERALIST_H
