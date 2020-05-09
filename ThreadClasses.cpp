#include "MainWindow.h"

PaintImage_Thread::PaintImage_Thread(ChildWindow* const window)
    : Window(window)
{}

void PaintImage_Thread::run()
{
    static QPainter painter;
    static QMutex mut;

    //Prevent Painting to the two images at the same time
    while(!Window->Image_Paint){};

    //Lock resources and prevent other thread from painting other Image
    mut.lock();
    Window->Image_Paint = false;

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

    Window->Image_Paint = true;
    mut.unlock();

    //Updates LastPoint
    Window->LastPoint = point;
}

void PaintImage_Thread::setPoint(const QPoint & p)
{
    point = p;
}

void PaintImage_Thread::setToggle(const bool t)
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
            mutex.lock();
            Window->ClearImage();
            mutex.unlock();
        {OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0; i = 0;}
            break;

        case 3:
            mutex.lock();
            Window->close();
            mutex.unlock();
            break;

        case 4:
        {i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;}
            break;

        case 5:
        {i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;}
            break;

        case 6:
        {i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;}
            break;

        case 7:
        {i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;}
            break;

        }
    }
}

void childReceive::setBIT(const bool b)
{
    bit = b;
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

