#include "ChildWindow.h"

ChildWindow::ChildWindow(QWidget* const parent)
    : QWidget(parent), initiate(true),  brush(Qt::darkGreen, Qt::SolidPattern), pen(brush, 4, Qt::DotLine, Qt::RoundCap),
      Image(1, 1, QImage::Format_RGB32)
{
    ImageThread = new ChildImage_Thread(this);
    BCP_ReceiveThread = new childReceive(this);
}

ChildWindow::~ChildWindow()
{
    ImageThread->deleteLater();
    BCP_ReceiveThread->deleteLater();
    delete ImageThread;
    delete BCP_ReceiveThread;
}

void ChildWindow::paintEvent(QPaintEvent *event)
{
    static QPainter painter;
    static QMutex mutex;

    //Paints Image on window
    mutex.lock();
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(event->rect(), Image, event->rect());
    painter.end();
    mutex.unlock();    
}

void ChildWindow::resizeEvent(QResizeEvent *event)
{
    static QMutex mutex;

    if(initiate)
    {
        mutex.lock();
        if( (Image.width()<event->size().width()) || (Image.height()<event->size().height()) )
        {
            int16_t oldX = event->oldSize().width();
            int16_t oldY = event->oldSize().height();
            Image = Image.copy(0,0,event->size().width(),event->size().height());

            static QPainter painter;
            painter.begin(&Image);
            painter.fillRect(QRect(QPoint(0,oldY), QSize(event->size().width(),(event->size().height()-oldY))), Qt::white);
            painter.fillRect(oldX,0,(event->size().width()-oldX),oldY,Qt::white);
            painter.end();
        }
        mutex.unlock();
    }
    else
        event->ignore();
}

static bool doClose(false);
void ChildWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Alt)
        doClose = true;
    if((event->key() == Qt::Key_F4) && doClose)
    {
        doClose = false;
        close();
    }
}

void ChildWindow::keyReleaseEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Alt)
        doClose = false;
}

void ChildWindow::PaintPoint(const QPoint & point)
{
    static QMutex mutex;

    while(ImageThread->isRunning()){}
    mutex.lock();
    ImageThread->setPoint(point);
    ImageThread->setToggle(ChildImage_Thread::POINT);
    mutex.unlock();
    ImageThread->start();
}

void ChildWindow::PaintLine(const QPoint & point)
{
    static QMutex mutex;

    while(ImageThread->isRunning()){}
    mutex.lock();
    ImageThread->setPoint(point);
    ImageThread->setToggle(ChildImage_Thread::LINE);
    mutex.unlock();
    ImageThread->start();
}

void ChildWindow::ClearImage()
{
    static QMutex mutex;

    //Clears Image, by making it all white and calls window PaintEvent
    mutex.lock();
    Image.fill(Qt::white);
    mutex.unlock();
    update();
}
