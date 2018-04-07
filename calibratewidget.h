#ifndef CALIBRATEWIDGET_H
#define CALIBRATEWIDGET_H

#include <QWidget>

namespace Ui {
class CalibrateWidget;
}

class CalibrateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrateWidget(QWidget *parent = 0);
    ~CalibrateWidget();

private:
    Ui::CalibrateWidget *ui;
};

#endif // CALIBRATEWIDGET_H
