#include "ThreadClasses.h"
#include "MainWindow.h"
#include <QPainter>

PaintImage_Thread::PaintImage_Thread(ChildWindow* window)
    : Window(window)
{}

void PaintImage_Thread::run()
{
    static QPainter painter;
    static QMutex mut;

    //Prevent Painting to the two images at the same time
    while(!Window->Image_Paint){};
    mut.lock();
    Window->Image_Paint = false;

    //Paints Line between current point and recorded last point, using current pen
    painter.begin(&(Window->Image));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Window->pen);
    if(Line_Point)
        painter.drawLine(point,Window->LastPoint);
    else
        painter.drawPoint(point);
    painter.end();
    mut.unlock();
    Window->Image_Paint = true;

    //Updates LastPoint
    Window->LastPoint = point;
}

void PaintImage_Thread::setPoint(const QPoint & p)
{
    point = p;
}

void PaintImage_Thread::setToggle(const bool t)
{
    Line_Point = t;
}

