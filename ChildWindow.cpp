#include "ChildWindow.h"

//Static variables
QPen ChildWindow::pen(Qt::darkGreen, 10, Qt::SolidLine, Qt::RoundCap);
bool ChildWindow::Image_Paint(true);

ChildWindow::ChildWindow(QWidget *parent)
    : QWidget(parent, Qt::Window), Image(X_leng, Y_leng, QImage::Format_RGB32)
{
    ImageThread = new PaintImage_Thread(this);

    //Once finished wihth writing to Image, update window
    QObject::connect(ImageThread, &PaintImage_Thread::finished,
                     this, &ChildWindow::ImagePaint_finished);

    setFixedSize(X_leng,Y_leng);
    Image.fill(Qt::white);
}

ChildWindow::~ChildWindow()
{
    ImageThread->deleteLater();
    delete ImageThread;
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

void ChildWindow::PaintPoint(QPoint point)
{
    ImageThread->setPoint(point);
    ImageThread->setToggle(PaintImage_Thread::POINT);
    ImageThread->start();
}

void ChildWindow::PaintLine(QPoint point)
{
    ImageThread->setPoint(point);
    ImageThread->setToggle(PaintImage_Thread::LINE);
    ImageThread->start();
}

void ChildWindow::ClearImage()
{
    //Clears Image, by making it all white and calls window PaintEvent
    Image.fill(Qt::white);
    update();
}

void ChildWindow::CloseWindow()
{
    close();
}

void ChildWindow::ImagePaint_finished()
{
    update();
}
