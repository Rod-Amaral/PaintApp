#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

#include <QPen>
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>
#include <QDebug>
#include "ThreadClasses.h"

//Child window class
class ChildWindow : public QWidget
{
    Q_OBJECT

public:
    ChildWindow(QWidget *parent = 0);
    ~ChildWindow();

    //Holds pen details, used in painting
    static QPen pen;

    //Window size, for now
	static const uint16_t X_leng = 700;
	static const uint16_t Y_leng = 800;

    //Holds last recorded point for drawing a line
    QPoint LastPoint;

    //Where the Image is held
    QImage Image;

    //Worker thread for Image painting,
    //and toggle variable so program doesn't paint on both Images at the same time
    PaintImage_Thread* ImageThread;
    static bool Image_Paint;

    void PaintPoint(const QPoint point);
    void PaintLine(const QPoint point);

    void ClearImage();
    void ImagePaint_finished();

protected:
    void paintEvent(QPaintEvent *event);


public slots:
    void IN_BIT(bool bit);

};

#endif
