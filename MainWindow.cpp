#include "MainWindow.h"
#include <QMutex>

MainWindow::MainWindow(ChildWindow *parent) : ChildWindow(parent)
{
    setFixedSize(X_leng+100,Y_leng);
}

MainWindow::~MainWindow()
{}

void MainWindow::paintEvent(QPaintEvent *event)
{
    static QPainter painter;
    static QMutex mut;

    //Paints Image on window
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    mut.lock();
    painter.drawImage(event->rect(), Image, event->rect());
    mut.unlock();

    //Side bar
    painter.setPen(QPen(Qt::black, 6, Qt::SolidLine, Qt::SquareCap));
    painter.drawLine(X_leng+2,0,X_leng+2,Y_leng);

    painter.end();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    static QPainter painter;
    static QPoint CurrentPoint;

    //Paints line on window and emits the same command for the other window
    if(ImageThread->isFinished())
    {
        CurrentPoint.setX(event->x());
        CurrentPoint.setY(event->y());
        PaintLine(CurrentPoint);
        emit doPaintLine(CurrentPoint);
    }
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    static QPainter painter;
    static QPoint CurrentPoint;

    //Paints point on window and emits the same command for the other window
    if(ImageThread->isFinished() | LastPoint.isNull())
    {
        CurrentPoint.setX(event->x());
        CurrentPoint.setY(event->y());
        PaintPoint(CurrentPoint);
        emit doPaintPoint(CurrentPoint);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //If R is pressed, clear Image and emits signal for other window to do the same
    if(event->key() == Qt::Key_R)
    {
        while(ImageThread->isRunning()) {}
            Image.fill(Qt::white);
            emit doclearImage();
            update();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit doCloseWindow();
}


