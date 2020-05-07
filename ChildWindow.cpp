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
    static QMutex mutex;
    //Clears Image, by making it all white and calls window PaintEvent
    mutex.lock();
    Image.fill(Qt::white);
    mutex.unlock();
    update();
}

void ChildWindow::ImagePaint_finished()
{
    update();
}

void ChildWindow::IN_BIT(bool bit)
{
    static bool OP_or_DATA = false;
    static uint8_t OP_code = 0;
    static int16_t data1 = 0, data2 = 0;
    static uint8_t i = 0;

    if(!OP_or_DATA)
    {
        if(i<3)
        {
            OP_code += (bit<<i);
            i++;
        }
        else
        {
            qDebug() << OP_code;
            OP_or_DATA = true;
            i = 0;
        }
    }

    if(OP_or_DATA)
    {
        //qDebug() << OP_code;
        switch(OP_code)
        {
        case 0:
            qDebug() << "case: 0";
            if(i<16)
            {
                data1 += (bit<<i);
                i++;
            }
            else if(i<32 && i>15)
            {
                data2 += (bit << (i-16));
                i++;
                if(i==32)
                    {PaintPoint(QPoint(data1,data2));
                    i = 0; OP_or_DATA = false; OP_code = 0; data1 = 0; data2 = 0;}
            }
            break;

        case 1:
            if(i<16)
            {
                data1 += (bit<<i);
                i++;
            }
            else if(i<32 && i>15)
            {
                data2 += (bit << (i-16));
                i++;
                if(i==32)
                    {PaintLine(QPoint(data1,data2));
                    i = 0; OP_or_DATA = false; OP_code = 0; data1 = 0; data2 = 0;}
            }
                break;

        case 2:
            ClearImage();
            qDebug() << "Cleared!";
        {OP_or_DATA = false; OP_code = 0; data1 = 0; data2 = 0; i = 0;}
            break;

        case 3:
            close();
        {OP_or_DATA = false; OP_code = 0; data1 = 0; data2 = 0; i = 0;}
            break;

        case 4:
        {i = 0; OP_or_DATA = false; OP_code = 0; data1 = 0; data2 = 0;}
            break;

        case 5:
        {i = 0; OP_or_DATA = false; OP_code = 0; data1 = 0; data2 = 0;}
            break;

        case 6:
        {i = 0; OP_or_DATA = false; OP_code = 0; data1 = 0; data2 = 0;}
            break;

        case 7:
        {i = 0; OP_or_DATA = false; OP_code = 0; data1 = 0; data2 = 0;}
            break;

        }
    }
}
/*
 OP Codes
 0: PaintPoint
 1: PaintLine
 2: ClearWindow
 3: ExitProgram
*/
