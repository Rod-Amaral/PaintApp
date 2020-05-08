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

    //Used to fix bug, where paintline is used, before paintpoint
    if(Window->LastPoint.isNull())
        Window->LastPoint = point;

    //Paints Line or Point, using current pen
    painter.begin(&(Window->Image));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Window->pen);
    if(Line_Point)
        painter.drawLine(point,Window->LastPoint);
    else
        painter.drawPoint(point);
    painter.end();

    Window->Image_Paint = true;
    mut.unlock();

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

