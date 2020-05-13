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
    Window->update();
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

bool parityCalculation(const uint8_t OP_code, const uint16_t data1, const uint16_t data2)
{
    bool parity(0);

    for(size_t i = 0; i<3; i++)
    {
        if((OP_code>>i)&1)
                parity = !parity;
    }
    for(size_t i = 0; i<16; i++)
    {
        if((data1>>i)&1)
                parity = !parity;
        if((data2>>i)&1)
                parity = !parity;
    }
    // 0 for even, 1 for odd
    return parity;
}

//Ok, this is kinda messy, but it has to be
void childReceive::run()
{
    static const uint8_t OP_code_bitsize(3);

    static bool OP_or_DATA(true); //used to determine to wether receiving OP_code or data
    static bool executeCommand(false);
    static uint8_t i(0); //Used to keep track of which bit is being received

    static uint8_t OP_code(0);
    static int16_t data1(0), data2(0);
    static bool parity(0);

    static QMutex mutex;

    //qDebug() << "First Send:\n" << "OP_code: " << OP_code << " data1: " << data1 << " data2: " << data2;

    //First 3 bits received, determine the OP code, 4th for parity
    if(OP_or_DATA)
    {
        if(i<OP_code_bitsize)
        {
            OP_code += (bit<<i);
            i++;
        }
        else if(i == OP_code_bitsize) //parity
        {
            parity = bit;
            i++;
        }
        else
        {
            OP_or_DATA = false;
            i = 0;
        }
    }

    //Here we receive data, if any
    if(!OP_or_DATA)
    {
        //No data to be received
        if(OP_code==2 || OP_code==3 || OP_code==7)
            executeCommand = true;
        //data to be received
        else
        {
            if(i<16)
            {
                data1 += (bit<<i);
                i++;
            }
            else
            {
                data2 += (bit << (i-16));
                i++;
                if(i==32)
                    executeCommand = true;
            }
        }
    }

    if(executeCommand)
    {
        static bool parityFail_once(true);
        if(parityFail_once)
        {
            qDebug() << "parity fail!";
            OP_code++;
            parityFail_once = false;
        }
        if(parity == parityCalculation(OP_code,data1,data2))
        {
            qDebug() << "Parity Success!";
            qDebug() << "OP_code: " << OP_code << " data1: " << data1 << " data2: " << data2;
            switch(OP_code)
            {
            case 0:             //DrawPoint, OPcode: 0  data1->x-coordinate  data2->y-coordinate
                Window->PaintPoint(QPoint(data1,data2));
                break;

            case 1:             //DrawLine, OPcode: 1  data1->x-coordinate  data2->y-coordinate
                Window->PaintLine(QPoint(data1,data2));
                break;

            case 2:             //ClearScreen, OPcode: 2  no data
                Window->ClearImage();
                break;

            case 3:             //CloseWindow, OPcode: 3 no data
                Window->close();
                break;

            case 4:             //SetBrushPen, OPcode: 4  see data info below
                mutex.lock();
                Window->brush.setStyle(Qt::BrushStyle(data1 & 0b11111));
                Window->pen.setBrush(Window->brush);
                Window->pen.setStyle(Qt::PenStyle( ((data1>>5) & 0b111) ));
                Window->pen.setWidth(data1>>8);
                Window->pen.setCapStyle(Qt::PenCapStyle( (data2 & 0b11)*0x10 ));
                //This following one is good ;)   check Qt:PenJoinStyle enum values
                Window->pen.setJoinStyle(Qt::PenJoinStyle( ((data2 & 0b1100)!=0b1100) ? ((data2 & 0b1100)*0x10) : (0x100) ));
                mutex.unlock();
            /*
                data1 bits->
                [1:5] BrushStyle, [6:8] PenStyle, [9:16] pen Width
                data2 bits->
                [1:2] PenCapStyle, [3:4] PenjoinStyle
            */
                break;

            case 5:             //ChangeColour, OPcode: 5  data1->LS bytes  data2->MS bytes,  32bit value total
                mutex.lock();
                Window->brush.setColor(((uint16_t)data1 + ((uint16_t)data2 << 16)));
                Window->pen.setBrush(Window->brush);
                mutex.unlock();
                break;

            case 6:             //Resize, OPcode: 6  data1->x-coordinate  data2->y-coordinate
                Window->showNormal();
                Window->resize(data1, data2);
                break;

            case 7:             //FullScreen, OPcode 7  no data
                Window->showFullScreen();
                break;

            }
            Window->PARITY_SEND(0);
        }
        else //If parity failed
        {
            qDebug() << "Parity Failed!";
            qDebug() << "OP_code: " << OP_code << " data1: " << data1 << " data2: " << data2;
            Window->PARITY_SEND(1);
        }
        i = 0; OP_or_DATA = true; executeCommand = false; OP_code = 0; data1 = 0; data2 = 0; parity = 0;
    }
}

void childReceive::setBIT(const bool b)
{
    bit = b;
}
