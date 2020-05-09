#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

#include <QPen>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include "ChildThreads.h"

//Child window class
class ChildWindow : public QWidget
{
    Q_OBJECT

public:
    ChildWindow(QWidget* const parent = 0);
    ~ChildWindow();

    //Holds pen details, used in painting
    QBrush brush;
    QPen pen;

    //Window size, for now
    const uint16_t X_leng = 700;
    const uint16_t Y_leng = 800;

    //Holds last recorded point for drawing a line
    QPoint LastPoint;

    //Where the Image is held
    QImage Image;

    //Worker thread for Image painting,
    ChildImage_Thread* ImageThread;

    //Worker thread for receiving data with the BCP
    childReceive* BCP_ReceiveThread;

    void PaintPoint(const QPoint & point);
    void PaintLine(const QPoint & point);

    void ClearImage();
    void ImagePaint_finished();

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:
    void IN_BIT(const bool bit);

};

#endif
