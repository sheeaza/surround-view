#include "cameralist.h"
#include "ui_cameralist.h"

CameraList::CameraList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraList)
{
    ui->setupUi(this);
}

CameraList::~CameraList()
{
    delete ui;
}
