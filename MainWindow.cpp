#include "MainWindow.h"

MainWindow::MainWindow(ChildWindow *parent) : ChildWindow(parent)
{
    setFixedSize(X_leng,Y_leng);
    painter.setPen(pen);
}

MainWindow::~MainWindow()
{}

void MainWindow::paintEvent(QPaintEvent *event)
{
    static uint16_t i,j;
    for(j=0; j<Y_leng; j++)
    {
        for(i=0; i<X_leng; i++)
            if(PixelMap[j][i])
            {
                painter.begin(this);
                painter.setPen(pen);
                painter.drawPoint(i,j);
                painter.end();
            }
    }
    update();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if( ((event->x()<X_leng && event->y()<Y_leng && event->x()>0 && event->y()>0) && (!PixelMap[event->y()][event->x()])) )
        setPoint( event->x(),event->y() );
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    setPoint( event->x(),event->y() ) ;
}
