#include "MainWindow.h"
#include <QTextStream>

static QTextStream out(stderr);

MainWindow::MainWindow(ChildWindow *parent) : ChildWindow(parent)
{}

MainWindow::~MainWindow()
{}

void MainWindow::paintEvent(QPaintEvent *event)
{
    painter.begin(this);
    painter.drawImage(0,0, Image, 0,0, -1,-1, Qt::AutoColor);
    painter.end();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    static uint16_t j,i;

    point = QPoint(event->x(), event->y());
    uint8_t pen_r = pen.width()/2;
    if( ( (point.x()+pen_r))<X_leng && (point.y()+pen_r)<Y_leng && point.x()>pen_r && point.y()>pen_r )
    {
        for(j=(point.y() - pen_r); j<(point.y() + pen_r) ; j++)
        {
            for(i=(point.x() - pen_r); i<(point.x() + pen_r) ; i++)
            {
                //if(Image.pixel(i,j) != Image.color(c_colour)) //Slows it down
                    Image.setPixel(i,j,c_colour);
            }
        }
        setPoint( point );
        update();
    }
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    static uint16_t j,i;

    point = QPoint(event->x(), event->y());
    uint8_t pen_r = pen.width()/2;
    if( ( (point.x()+pen_r))<X_leng && (point.y()+pen_r)<Y_leng && point.x()>pen_r && point.y()>pen_r )
    {
        for(j=(point.y() - pen_r); j<(point.y() + pen_r) ; j++)
        {
            for(i=(point.x() - pen_r); i<(point.x() + pen_r) ; i++)
            {
                //if(Image.pixel(i,j) != Image.color(c_colour))
                    Image.setPixel(i,j,c_colour);
            }
        }
        setPoint( point );
        update();
    }
}
