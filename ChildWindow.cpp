#include "ChildWindow.h"
#include <QTextStream>

static QTextStream out(stderr); //Debug

static const uint8_t Pen_size = 5;
static const QBrush Brush = Qt::darkGreen; 

//Static variables
QPen ChildWindow::pen(Brush, Pen_size);

ChildWindow::ChildWindow(QWidget *parent)
    : QWidget(parent, Qt::Window), c_colour(1), Image(X_leng, Y_leng, QImage::Format_Indexed8)
{
	setFixedSize(X_leng,Y_leng);
    Image.setColor(0,qRgb(255,255,255));
    Image.setColor(1,qRgb(0,0,0));
}

ChildWindow::~ChildWindow()
{}

void ChildWindow::paintEvent(QPaintEvent *event)
{
    painter.begin(this);
    painter.drawImage(0,0, Image, 0,0, -1,-1, Qt::AutoColor);
    painter.end();
    update();
}

void ChildWindow::setPoint(QPoint point)
{
    static uint16_t i,j;
    for(j=(point.y() - pen.width()/2); j<(point.y() + pen.width()/2) ; j++)
    {
        for(i=(point.x() - pen.width()/2); i<(point.x() + pen.width()/2) ; i++)
            Image.setPixel(i,j,1);
    }
    emit pointChanged(point);
}




