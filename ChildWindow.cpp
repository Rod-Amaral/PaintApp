#include "ChildWindow.h"

//Static variables
QPen ChildWindow::pen(Qt::darkGreen, 10, Qt::SolidLine, Qt::RoundCap);

ChildWindow::ChildWindow(QWidget *parent)
    : QWidget(parent, Qt::Window), Image(X_leng, Y_leng, QImage::Format_RGB32)
{
	setFixedSize(X_leng,Y_leng);
    Image.fill(Qt::white);
}

ChildWindow::~ChildWindow()
{}

void ChildWindow::paintEvent(QPaintEvent *event)
{
    static QPainter painter;

    //Paints Image on window
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(event->rect(), Image, Image.rect());
    painter.end();
}

void ChildWindow::PaintPoint(QPoint point)
{
    static QPainter painter;

    //Paints Point on Image with current Pen
    painter.begin(&Image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(pen);
    painter.drawPoint(point);
    painter.end();

    //Updates LastPoint, and call PaintEvent
    LastPoint = point;
    update();
}

void ChildWindow::PaintLine(QPoint point)
{
    static QPainter painter;

    //Paints Line between current point and recorded last point, using current pen
    painter.begin(&Image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(pen);
    painter.drawLine(point,LastPoint);
    painter.end();

    //Updates LastPoint, and call window PaintEvent
    LastPoint = point;
    update();
}

void ChildWindow::ClearImage()
{
    //Clears Image, by making it all white and calls window PaintEvent
    Image.fill(Qt::white);
    update();
}
