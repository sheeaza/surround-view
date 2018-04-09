#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QDir>
#include <QFileInfo>

#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    intrinsicAllReady(0),
    extrinsicAllReady(0),
    curFile("./data/cameras.xml")
{
    ui->setupUi(this);

    QFileInfo checkFile(curFile);

    QVector<QString> camName;
    camName << "front" << "left" << "back" << "right";
    foreach (const QString &name, camName) {
        CameraParameter p;
        p.name = name;
        camParameters.append(p);
    }

    if(checkFile.exists() && checkFile.isFile()) {
        load();
    } else {
        qInfo() << QDir(curFile).absolutePath() << ": not found";
        save();
    }
}

MainWindow::~MainWindow()
{
    if(CameraParameter::changed) {
        save();
    }
    delete ui;
}

void MainWindow::on_intrinButton_clicked()
{
    cameraList = new CameraListWidget(CameraListWidget::intrinsic,
                                         camParameters, this);
    cameraList->setAttribute(Qt::WA_DeleteOnClose);
    cameraList->setWindowTitle("Intrinsic List");
    cameraList->exec();
}

void MainWindow::on_extrinButton_clicked()
{
    cameraList = new CameraListWidget(CameraListWidget::extrinsic,
                                         camParameters, this);
    cameraList->setAttribute(Qt::WA_DeleteOnClose);
    cameraList->setWindowTitle("Extrinsic List");
    cameraList->exec();
}

void MainWindow::on_svButton_clicked()
{
    svDialog = new SurroundViewDialog(camParameters, this);

    svDialog->setAttribute(Qt::WA_DeleteOnClose);
    svDialog->setWindowTitle("Surround View");
    svDialog->exec();
}

void MainWindow::save()
{
    cv::FileStorage fs(curFile.toStdString(),
                       cv::FileStorage::WRITE);
    fs << "chessboardSetting" << "{"
       << "width" << CameraParameter::boardSize.width
       << "height" << CameraParameter::boardSize.height
       << "squareSize" << CameraParameter::squareSize
       << "}";

    fs << "imageResolution" << "{"
       << "width" << CameraParameter::imgSize.width
       << "height" << CameraParameter::imgSize.height
       << "}";

    fs << "parameters" << "{";
    foreach (const CameraParameter &p, camParameters) {
        fs << p.name.toStdString() << "{"
           << "devId" << p.devId
           << "intrinsicReady" << p.intrinsicDistReady
           << "extrinsicReady" << p.extrinsicReady
           << "intrinsic" << p.cvIntrinsic
           << "dist" << p.cvDistCoeffs
           << "extrinsic" << p.cvExtrinsic
           << "}";
    }
    fs << "}";
}

void MainWindow::load()
{
    cv::FileStorage fs(curFile.toStdString(),
                       cv::FileStorage::READ);

    cv::FileNode n;
    n = fs["chessboardSetting"];
    n["width"] >> CameraParameter::boardSize.width;
    n["height"] >> CameraParameter::boardSize.height;
    n["squareSize"] >> CameraParameter::squareSize;

    n = fs["imageResolution"];
    n["width"] >> CameraParameter::imgSize.width;
    n["height"] >> CameraParameter::imgSize.height;

    n = fs["parameters"];

    for(QVector<CameraParameter>::iterator it = camParameters.begin();
        it != camParameters.end(); ++it) {
        cv::FileNode sn = n[it->name.toStdString()];
        sn["devId"] >> it->devId;
        sn["intrinsicReady"] >> it->intrinsicDistReady;
        sn["extrinsicReady"] >> it->extrinsicReady;
        sn["intrinsic"] >> it->cvIntrinsic;
        sn["dist"] >> it->cvDistCoeffs;
        sn["extrinsic"] >> it->cvExtrinsic;
    }
}
