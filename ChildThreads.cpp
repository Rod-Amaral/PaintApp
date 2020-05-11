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
            else
            {
                data2 += (bit << (i-16));
                i++;

                if(i==32)
                {
                    Window->PaintPoint(QPoint(data1,data2));
                    Window->oldX = Window->width(); Window->oldY = Window->height();
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
            else
            {
                data2 += (bit << (i-16));
                i++;

                if(i==32)
                {
                    Window->PaintLine(QPoint(data1,data2));
                    i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;
                }
            }
                break;

        case 2:
            Window->ClearImage();
            OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0; i = 0;
            break;

        case 3:
            Window->close();
            break;

        case 4:
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
                {
                    mutex.lock();
                    Window->brush.setStyle(Qt::BrushStyle(data1 & 0b11111));
                    Window->pen.setBrush(Window->brush);
                    Window->pen.setStyle(Qt::PenStyle( ((data1>>5) & 0b111) ));
                    Window->pen.setWidth(data1>>8);
                    Window->pen.setCapStyle(Qt::PenCapStyle( (data2 & 0b11)*0x10 ));
                    //This following one is good ;)   check Qt:PenJoinStyle enum values
                    Window->pen.setJoinStyle(Qt::PenJoinStyle( ((data2 & 0b1100)!=0b1100) ? ((data2 & 0b1100)*0x10) : (0x100) ));
                    mutex.unlock();
                    i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;
                }
                /*
                    data1 bits->
                    [1:5] BrushStyle, [6:8] PenStyle, [9:16] pen Width
                    data2 bits->
                    [1:2] PenCapStyle, [3:4] PenjoinStyle
                */
            }
            break;

        case 5:
            if(i<16)
            {
                data1 += (bit<<i);
                i++;
            }
            else
            {
                data2 += (bit<<(i-16));
                i++;

                if(i==32)
                {
                    mutex.lock();
                    Window->brush.setColor(((uint16_t)data1 + ((uint16_t)data2 << 16)));
                    Window->pen.setBrush(Window->brush);
                    mutex.unlock();
                    i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;
                }
            }
            break;

        case 6:
            if(i<16)
            {
                data1 += (bit<<i);
                i++;
            }
            else
            {
                data2 += (bit<<(i-16));
                i++;

                if(i==32)
                {
                    mutex.lock();
                    Window->resize(data1, data2);
                    if( (Window->Image.width()<Window->width()) || (Window->Image.height()<Window->height()) )
                    {
                        Window->Image = Window->Image.copy(0,0,Window->width(),Window->height());

                        static QPainter painter;
                        painter.begin(&Window->Image);
                        painter.fillRect(0,Window->oldY,Window->width(),(Window->height()-Window->oldY),Qt::white);
                        painter.fillRect(Window->oldX,0,(Window->width()-Window->oldX),Window->oldY,Qt::white);
                        painter.end();
                    }
                    static bool once = true;
                    if(once)
                    {
                        Window->Image.fill(Qt::white);
                        once = false;
                    }
                    mutex.unlock();
                    Window->update();
                    i = 0; OP_or_DATA = true; OP_code = 0; data1 = 0; data2 = 0;
                }
            }
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
