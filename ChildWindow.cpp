#include "ChildWindow.h"

//Static variables
QPen ChildWindow::pen(Qt::darkGreen, 4, Qt::SolidLine, Qt::RoundCap);
bool ChildWindow::Image_Paint(true);

ChildWindow::ChildWindow(QWidget* const parent)
    : QWidget(parent, Qt::Window), Image(X_leng, Y_leng, QImage::Format_RGB32)
{
    ImageThread = new PaintImage_Thread(this);
    BCP_ReceiveThread = new childReceive(this);

    //Once finished wihth writing to Image, update window
    QObject::connect(ImageThread, &QThread::finished,
                     this, &ChildWindow::ImagePaint_finished);

    setFixedSize(X_leng,Y_leng);
    Image.fill(Qt::white);
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
    static QMutex mut;

    //Paints Image on window
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    mut.lock();
    painter.drawImage(event->rect(), Image, event->rect());
    mut.unlock();
    painter.end();
}

void ChildWindow::PaintPoint(const QPoint & point)
{
    ImageThread->setPoint(point);
    ImageThread->setToggle(PaintImage_Thread::POINT);
    ImageThread->start();
}

void ChildWindow::PaintLine(const QPoint & point)
{
    ImageThread->setPoint(point);
    ImageThread->setToggle(PaintImage_Thread::LINE);
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

//I couldn't slot update directly -_-
void ChildWindow::ImagePaint_finished()
{
    update();
}

void ChildWindow::IN_BIT(const bool bit)
{
    while(BCP_ReceiveThread->isRunning()){}
    BCP_ReceiveThread->setBIT(bit);
    BCP_ReceiveThread->start();
}
