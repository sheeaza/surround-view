#include "extrinsicdialog.h"
#include "ui_extrinsicdialog.h"

ExtrinsicDialog::ExtrinsicDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtrinsicDialog)
{
    ui->setupUi(this);
}

ExtrinsicDialog::~ExtrinsicDialog()
{
    delete ui;
}
