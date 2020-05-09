#include "MainWindow.h"

MainImage_Thread::MainImage_Thread(MainWindow* const window)
    : Window(window)
{}

void MainImage_Thread::run()
{
    static QPainter painter;
    static QMutex mutex;

    mutex.lock();

    //Used to fix bug, where paintline is used, before paintpoint
    if(Window->LastPoint.isNull())
        Window->LastPoint = point;

    //Paints Line or Point, using current pen
    painter.begin(&(Window->Image));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Window->pen);
    if(Line_Point)
        painter.drawLine(point,Window->LastPoint);
    else
        painter.drawPoint(point);
    painter.end();

    //Updates LastPoint
    Window->LastPoint = point;

    mutex.unlock();
}

void MainImage_Thread::setPoint(const QPoint & p)
{
    point = p;
}

void MainImage_Thread::setToggle(const bool t)
{
    Line_Point = t;
}




mainSend::mainSend(MainWindow* const window)
    : Window(window), OP_code(0), data1(0), data2(0)
{}


void mainSend::run()
{
    static QMutex mutex;
    static uint8_t i;

    mutex.lock();
    emit Window->SEND_BIT((bool)(OP_code & 1));
    emit Window->SEND_BIT((bool)(OP_code & 2));
    emit Window->SEND_BIT((bool)(OP_code & 4));

    if(OP_code>1)
        emit Window->SEND_BIT(0);
    else
    {
        for(i = 0; i<16; i++)
            emit Window->SEND_BIT((bool)1&(data1 >> i));
        for(i = 0; i<16; i++)
            emit Window->SEND_BIT((bool)1&(data2 >> i));
    }
    mutex.unlock();
}

void mainSend::setOP_code(const uint8_t op)
{
    OP_code = op;
}

void mainSend::setData1(const int16_t data)
{
    data1 = data;
}

void mainSend::setData2(const int16_t data)
{
    data2 = data;
}

/*
 OP Codes
 0: PaintPoint
 1: PaintLine
 2: ClearWindow
 3: ExitProgram
*/

