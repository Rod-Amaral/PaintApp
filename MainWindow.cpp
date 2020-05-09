#include "MainWindow.h"

MainWindow::MainWindow(ChildWindow* const parent) : ChildWindow(parent)
{
    setFixedSize(X_leng+100,Y_leng);
    BCP_SendThread = new mainSend(this);
}

MainWindow::~MainWindow()
{
    delete BCP_SendThread;
}

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
        SendPaintLine(CurrentPoint);
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
        SendPaintPoint(CurrentPoint);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    static QMutex mutex;

    //If R is pressed, clear Image and emit signal for other window to do the same
    if(event->key() == Qt::Key_R)
    {
        mutex.lock();
        Image.fill(Qt::white);
        mutex.unlock();

        //Clear Image OP code
        BCP_SendThread->setOP_code(2);
        BCP_SendThread->start();

        update();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //Close Window OP code
    BCP_SendThread->setOP_code(3);
    BCP_SendThread->start();

    event->accept();
}

void MainWindow::SendPaintPoint(const QPoint & CurrentPoint)
{
    //Send Point OP code
    BCP_SendThread->setOP_code(0);
    BCP_SendThread->setData1(CurrentPoint.x());
    BCP_SendThread->setData2(CurrentPoint.y());
    BCP_SendThread->start();
}

void MainWindow::SendPaintLine(const QPoint & CurrentPoint)
{
    //Send Line OP code
    BCP_SendThread->setOP_code(1);
    BCP_SendThread->setData1(CurrentPoint.x());
    BCP_SendThread->setData2(CurrentPoint.y());
    BCP_SendThread->start();
}


