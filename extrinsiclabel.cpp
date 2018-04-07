#include "imagelabel.h"

#include <QMouseEvent>
#include <QDebug>

ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent)
{
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

void ImageLabel::mousePressEvent(QMouseEvent *ev)
{
    originPos = ev->pos();
//    qInfo() << originPos;
    rubberBand->setGeometry(QRect(originPos, QSize()));
    rubberBand->show();
}

void ImageLabel::mouseMoveEvent(QMouseEvent *ev)
{
    rubberBand->setGeometry(QRect(originPos, ev->pos()).normalized());
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    rubberBand->hide();
    endPos = ev->pos();
    emit mouseReleased(originPos, endPos);
}

