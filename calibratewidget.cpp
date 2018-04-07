#include "calibratewidget.h"
#include "ui_calibratewidget.h"

CalibrateWidget::CalibrateWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalibrateWidget)
{
    ui->setupUi(this);
}

CalibrateWidget::~CalibrateWidget()
{
    delete ui;
}
