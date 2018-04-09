#-------------------------------------------------
#
# Project created by QtCreator 2018-03-28T22:29:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = surround_view_synthesis
TEMPLATE = app

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
linux-oe-g++ {
    PKGCONFIG += opencv
}

linux-g++ {
    PKGCONFIG += /home/max/project/opencv-lib/lib/pkgconfig/opencv.pc
}
PKGCONFIG += assimp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    calibratewidget.cpp \
    cameralistwidget.cpp \
    cameraparameter.cpp \
    cameraCalibrator.cpp \
    extrinsicdialog.cpp \
    extrinsiclabel.cpp \
    camera3d.cpp \
    gpurender.cpp \
    mesh.cpp \
    model.cpp \
    v4l2capture.cpp \
    surroundviewdialog.cpp

HEADERS += \
        mainwindow.h \
    calibratewidget.h \
    cameralistwidget.h \
    cameraparameter.h \
    cameraCalibrator.h \
    extrinsicdialog.h \
    extrinsiclabel.h \
    camera3d.h \
    gpurender.h \
    mesh.h \
    model.h \
    v4l2capture.h \
    surroundviewdialog.h

FORMS += \
    calibratewidget.ui \
    mainwindow.ui \
    cameralistwidget.ui \
    extrinsicdialog.ui \
    surroundviewdialog.ui

RESOURCES += \
    icons.qrc \
    images.qrc \
    shaders.qrc

target.path = /home/root/surround_view_synthesis
INSTALLS += target
