#ifndef CAMERALIST_H
#define CAMERALIST_H

#include <QDialog>

namespace Ui {
class CameraList;
}

class CameraList : public QDialog
{
    Q_OBJECT

public:
    explicit CameraList(QWidget *parent = 0);
    ~CameraList();

private:
    Ui::CameraList *ui;
};

#endif // CAMERALIST_H
