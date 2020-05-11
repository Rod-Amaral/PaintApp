#include "ChildWindow.h"

ChildWindow::ChildWindow(QWidget* const parent)
    : QWidget(parent),  brush(Qt::darkGreen, Qt::SolidPattern), pen(brush, 4, Qt::DotLine, Qt::RoundCap),
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
    int16_t oldX = event->oldSize().width();
    int16_t oldY = event->oldSize().height();

    mutex.lock();
    if( (Image.width()<width()) || (Image.height()<height()) )
    {
        Image = Image.copy(0,0,width(),height());

        static QPainter painter;
        painter.begin(&Image);
        painter.fillRect(0,oldY,width(),(height()-oldY),Qt::white);
        painter.fillRect(oldX,0,(width()-oldX),oldY,Qt::white);
        painter.end();
    }
    static bool once = true;
    if(once)
    {
        Image.fill(Qt::white);
        once = false;
    }
    mutex.unlock();
}

void ChildWindow::PaintPoint(const QPoint & point)
{
    while(ImageThread->isRunning()){}
    ImageThread->setPoint(point);
    ImageThread->setToggle(ChildImage_Thread::POINT);
    ImageThread->start();
}

void ChildWindow::PaintLine(const QPoint & point)
{
    while(ImageThread->isRunning()){}
    ImageThread->setPoint(point);
    ImageThread->setToggle(ChildImage_Thread::LINE);
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

void ChildWindow::IN_BIT(const bool bit)
{
    while(BCP_ReceiveThread->isRunning()){}
    BCP_ReceiveThread->setBIT(bit);
    BCP_ReceiveThread->start();
}
