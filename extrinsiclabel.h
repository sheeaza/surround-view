#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>
#include <QRubberBand>

class ExtrinsicLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ExtrinsicLabel(QWidget *parent = nullptr);

protected:
//    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;

signals:
    void mouseReleased(const QPoint&, const QPoint&);

public slots:
private:
    QPoint originPos, endPos;
    QRubberBand *rubberBand;
};

#endif // IMAGELABEL_H
