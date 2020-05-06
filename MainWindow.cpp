#include "MainWindow.h"


MainWindow::MainWindow(ChildWindow *parent) : ChildWindow(parent)
{}

MainWindow::~MainWindow()
{}

void MainWindow::paintEvent(QPaintEvent *event)
{
    static QPainter painter;

    //Paints Image on window
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(event->rect(), Image, Image.rect());
    painter.end();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    static QPainter painter;
    QPoint CurrentPoint(event->x(), event->y());

    //Paints line on window and emits the same command for the other window
    PaintLine(CurrentPoint);
    emit doPaintLine(CurrentPoint);
    update();
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    static QPainter painter;
    QPoint CurrentPoint(event->x(), event->y());

    //Paints point on window and emits the same command for the other window
    PaintPoint(CurrentPoint);
    emit doPaintPoint(CurrentPoint);
    update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //If R is pressed, clear Image and emits signal for other window to do the same
    if(event->key() == Qt::Key_R)
    {
        Image.fill(Qt::white);
        update();
    }
    emit doclearImage();
}


