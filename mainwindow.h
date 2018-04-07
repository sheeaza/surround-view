#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPointer>

#include "cameralistwidget.h"
#include "cameraparameter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_intrinButton_clicked();

    void on_extrinButton_clicked();

    void on_svButton_clicked();

private:
    void save();
    void load();

    Ui::MainWindow *ui;
    bool intrinsicAllReady;
    bool extrinsicAllReady;

    CameraListWidget *intrinsicList;
    QVector<CameraParameter> camParameters;

    const QString curFile;
};

#endif // MAINWINDOW_H
