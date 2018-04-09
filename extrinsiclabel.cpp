#include "extrinsiclabel.h"

#include <QMouseEvent>
#include <QDebug>

ExtrinsicLabel::ExtrinsicLabel(QWidget *parent) : QLabel(parent)
{
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

void ExtrinsicLabel::mousePressEvent(QMouseEvent *ev)
{
    originPos = ev->pos();
//    qInfo() << originPos;
    rubberBand->setGeometry(QRect(originPos, QSize()));
    rubberBand->show();
}

void ExtrinsicLabel::mouseMoveEvent(QMouseEvent *ev)
{
    rubberBand->setGeometry(QRect(originPos, ev->pos()).normalized());
}

void ExtrinsicLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    rubberBand->hide();
    endPos = ev->pos();
    emit mouseReleased(originPos, endPos);
}

