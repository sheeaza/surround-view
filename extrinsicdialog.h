#ifndef EXTRINSICDIALOG_H
#define EXTRINSICDIALOG_H

#include <QDialog>

namespace Ui {
class ExtrinsicDialog;
}

class ExtrinsicDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExtrinsicDialog(QWidget *parent = 0);
    ~ExtrinsicDialog();

private:
    Ui::ExtrinsicDialog *ui;
};

#endif // EXTRINSICDIALOG_H
