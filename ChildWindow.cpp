#include "ChildWindow.h"
#include <QTextStream>

static QTextStream out(stderr); //Debug

static const uint8_t Pen_size = 20;
static const QBrush Brush = Qt::black;

//Static variables
QPen ChildWindow::pen(Brush, Pen_size);

ChildWindow::ChildWindow(QWidget *parent)
    : QWidget(parent, Qt::Window), Image(X_leng, Y_leng, QImage::Format_RGB32)
{
    pen.setCapStyle(Qt::RoundCap);
	setFixedSize(X_leng,Y_leng);
    Image.fill(Qt::white);
}

ChildWindow::~ChildWindow()
{}

void ChildWindow::paintEvent(QPaintEvent *event)
{
    static QPainter painter;
    painter.begin(this);
    painter.drawImage(event->rect(), Image, Image.rect());
    painter.end();
    update();
}
