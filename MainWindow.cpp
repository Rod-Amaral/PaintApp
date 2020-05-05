#include "MainWindow.h"
#include <QTextStream>

static QTextStream out(stderr);

MainWindow::MainWindow(ChildWindow *parent) : ChildWindow(parent)
{}

MainWindow::~MainWindow()
{}

void MainWindow::paintEvent(QPaintEvent *event)
{
    static QPainter painter;
    painter.begin(this);
    painter.drawImage(event->rect(), Image, Image.rect());
    painter.end();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    static const uint8_t pen_r = pen.width()/2;
    static QPainter painter;

    point = QPoint(event->x(), event->y());
    if( ( (point.x()+pen_r))<X_leng && (point.y()+pen_r)<Y_leng && point.x()>pen_r && point.y()>pen_r )
    {
        PaintPoint( point );
        update();
    }
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    static const uint8_t pen_r = pen.width()/2;
    static QPainter painter;

    point = QPoint(event->x(), event->y());
    if( ( (point.x()+pen_r))<X_leng && (point.y()+pen_r)<Y_leng && point.x()>pen_r && point.y()>pen_r )
    {
        PaintPoint( point );
        update();
    }
}
