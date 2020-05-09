#include "ChildWindow.h"

ChildImage_Thread::ChildImage_Thread(ChildWindow* const window)
    : Window(window)
{}

void ChildImage_Thread::run()
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

void ChildImage_Thread::setPoint(const QPoint & p)
{
    point = p;
}

void ChildImage_Thread::setToggle(const bool t)
{
    Line_Point = t;
}



childReceive::childReceive(ChildWindow* const window)
    : Window(window)
{}

//Ok, this is kinda messy, but it has to be
void childReceive::run()
{
    static bool OP_or_DATA = true;
    static uint8_t OP_code = 0;
    static int16_t data1 = 0, data2 = 0;
    static uint8_t i = 0;
    static QMutex mutex;

    //First 3 bits received, determine the OP code
    if(OP_or_DATA)
    {
        if(i<3)
        {
            OP_code += (bit<<i);
            i++;
        }
        else
        {
            OP_or_DATA = false;
            i = 0;
        }
    }

    //Bits sent after, execute the OP code command, or are intepreted as data
    if(!OP_or_DATA)
    {
        switch(OP_code)
        {
        case 0:
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
                {
                    mutex.lock();
                    Window->PaintPoint(QPoint(data1,data2));
                    mutex.unlock();
                    i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;
                }
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
                {
                    mutex.lock();
                    Window->PaintLine(QPoint(data1,data2));
                    mutex.unlock();
                    i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;
                }
            }
                break;

        case 2:
            Window->ClearImage();
            OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0; i = 0;
            break;

        case 3:
            mutex.lock();
            Window->close();
            mutex.unlock();
            break;

        case 4:
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
                {
                    //CHANGE PEN
                    i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;
                }
            }
            break;

        case 5:
            i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;
            break;

        case 6:
            i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;
            break;

        case 7:
            i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;
            break;

        }
    }
}

void childReceive::setBIT(const bool b)
{
    bit = b;
}
