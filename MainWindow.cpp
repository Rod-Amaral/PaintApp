#include "MainWindow.h"
#include <QMutex>
#include <QDebug>

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
        emit SendPaintLine(CurrentPoint);
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
        emit SendPaintPoint(CurrentPoint);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    static QMutex mutex;

    //If R is pressed, clear Image and emit signal for other window to do the same
    if(event->key() == Qt::Key_R)
    {
        while(ImageThread->isRunning()) {}
        mutex.lock();
        Image.fill(Qt::white);

        //Clear Image OP code
        emit SEND_BIT(0); emit SEND_BIT(1); emit SEND_BIT(0);
        mutex.unlock();
        update();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    static QMutex mutex;
    mutex.lock();

    //Close Window OP code
    emit SEND_BIT(1); emit SEND_BIT(1); emit SEND_BIT(0);
    mutex.unlock();
    event->accept();
}

void MainWindow::SendPaintPoint(QPoint CurrentPoint)
{
    static int16_t data;
    static QMutex mutex;
    mutex.lock();

    //Send Point OP code
    emit SEND_BIT(0); emit SEND_BIT(0); emit SEND_BIT(0);

    data = CurrentPoint.x();
    for(uint8_t i = 0; i<16; i++)
        emit SEND_BIT((bool)1&(data >> i));
    data = CurrentPoint.y();
    for(uint8_t i = 0; i<16; i++)
        emit SEND_BIT((bool)1&(data >> i));
    mutex.unlock();
}

void MainWindow::SendPaintLine(QPoint CurrentPoint)
{
    static int16_t data;
    static QMutex mutex;
    mutex.lock();

    //Send Line OP code
    emit SEND_BIT(1); emit SEND_BIT(0); emit SEND_BIT(0);

    data = CurrentPoint.x();
    for(uint8_t i = 0; i<16; i++)
        emit SEND_BIT((bool)1&(data >> i));
    data = CurrentPoint.y();
    for(uint8_t i = 0; i<16; i++)
        emit SEND_BIT((bool)1&(data >> i));
    mutex.unlock();
}


