#include "MainWindow.h"
#include <QTextStream>

static QTextStream out(stderr);

MainWindow::MainWindow(ChildWindow *parent) : ChildWindow(parent), no_paint(0)
{}

MainWindow::~MainWindow()
{}

void MainWindow::paintEvent(QPaintEvent *event)
{
    static QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(event->rect(), Image, Image.rect());
    painter.end();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    static const uint8_t pen_r = pen.width()/2;
    static QPainter painter;

    QPoint CurrentPoint(event->x(), event->y());

    if( ( (CurrentPoint.x()+pen_r))<X_leng && (CurrentPoint.y()+pen_r)<Y_leng && CurrentPoint.x()>pen_r && CurrentPoint.y()>pen_r )
    {
        if(no_paint)
        {
            PaintPoint( CurrentPoint );
            no_paint = 0;
        }
        else
            PaintLine( CurrentPoint );
        update();
    }
    else
        no_paint = true;
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    static const uint8_t pen_r = pen.width()/2;
    static QPainter painter;

    QPoint CurrentPoint(event->x(), event->y());

    if( ( (CurrentPoint.x()+pen_r))<X_leng && (CurrentPoint.y()+pen_r)<Y_leng && CurrentPoint.x()>pen_r && CurrentPoint.y()>pen_r )
    {
        PaintPoint( CurrentPoint );
        update();
        if(no_paint)
            no_paint = 0;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{

}

void ChildWindow::PaintPoint(QPoint point)
{
    static QPainter painter;

    painter.begin(&Image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(pen);
    painter.drawPoint(point);
    painter.end();
    LastPoint = point;
    emit pointChanged(point);
}

void ChildWindow::PaintLine(QPoint point)
{
    static QPainter painter;

    painter.begin(&Image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(pen);
    painter.drawLine(point,LastPoint);
    painter.end();
    LastPoint = point;
    emit pointChanged(point);
}
