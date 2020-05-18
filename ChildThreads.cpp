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

void Backtrack(uint16_t & i, uint16_t & j, const uint16_t maxI)
{
    if(i>1)
        i -= 2;
    else
    {
        if(i)
            i = maxI;
        else
            i = maxI-1;
        j--;
    }
}

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
    static const uint8_t OP_code_bitsize(4);

    static bool OP_or_DATA(true); //used to determine to wether receiving OP_code or data
    static bool executeCommand(false); //To signal, when data has been received
    static uint8_t i(0); //Used to keep track of which bit is being received

    static uint8_t OP_code(0);
    static int16_t data1(0), data2(0);
    static bool parity(0);

    //Used for Image Sync
    static uint16_t ii(0), jj(0);
    static uint16_t width(0);
    static uint16_t height(0);

    static QMutex mutex;

    while(1)
    {
        //Collection of data
        while(i<32)
        {
            mutex.lock();
            if(DO_READ)
            {
                //First 4 bits received, determine the OP code, 5th for parity
                if(OP_or_DATA)
                {
                    if(i<OP_code_bitsize)
                    {
                        OP_code += (BIT<<i);
                        i++;
                    }
                    else
                    {
                        parity = BIT;
                        i = 0;
                        OP_or_DATA = false;
                    }
                }
                else  //Here we receive data, if any
                {
                    //No data to be received
                    if(OP_code==2 || OP_code==3 || OP_code==7)
                    {
                        i = 31;
                    }

                    //Data to be received
                    if(i<16)
                        data1 += (BIT<<(i));
                    else
                    {
                        data2 += (BIT<<(i-16));
                        if(i==31)
                            executeCommand = true;
                    }
                    i++;
                }
                DO_READ = false;
            }
            mutex.unlock();
        }

        qDebug() << (OP_code) << '\n' << (bool)parity << '\n' << (uint32_t)data1 << '\n' << (uint32_t)data2;

        //Here we execute the command
        if(executeCommand)
        {
            static bool parityFail_once(true); //Used to make a parity error at the beginning

            if(parityFail_once)
            {
                OP_code++;
                parityFail_once = false;
            }

            if(parity == parityCalculation(OP_code,data1,data2))
            {
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
                    mutex.lock();
                    Window->resize(data1, data2);
                    mutex.unlock();
                    break;

                case 7:             //FullScreen, OPcode: 7  no data
                    Window->showFullScreen();
                    break;

                case 8:             //Set LastPoint, OPcode: 8  data1->x-coordinate  data2->y-coordinate
                    Window->LastPoint.setX(data1);
                    Window->LastPoint.setY(data2);
                    break;

                case 9:             //Sync Image data, OPcode: 9  data1->LS bytes  data2->MS bytes,  32bit value total
                    static bool getPixelAmount(true);

                    if(getPixelAmount)
                    {
                        width = data1;
                        height = data2;
                        getPixelAmount = false;
                        mutex.lock();
                        Window->initiate = true;
                        mutex.unlock();
                    }
                    else
                    {
                        Window->Image.setPixel(ii,jj,((uint16_t)data1)+(((uint16_t)data2)<<16));
                        if(ii<width)
                            ii++;
                        if(ii==width)
                        {
                            jj++;
                            ii = 0;
                        }

                    }
                    if(height == jj)
                    {
                        width = 0; height = 0; ii = 0; jj = 0; getPixelAmount = true;
                        mutex.lock();
                        Window->initiate = true;
                        mutex.unlock();
                        Window->update();
                    }
                    break;

                case 10:            //Skip Pixel, OPcode: 10  data1-> ii  dat2-> jj
                    ii = data1;
                    jj = data2;
                    if(jj == height)
                    {
                        width = 0; height = 0; ii = 0; jj = 0; getPixelAmount = true;
                        mutex.lock();
                        Window->initiate = true;
                        mutex.unlock();
                        Window->update();
                    }
                }
                RESEND_SEND(false);
            }
            else //If parity failed
            {
                qDebug() << "Parity Failed!";
                qDebug() << "OP_code: " << OP_code << " data1: " << data1 << " data2: " << data2 << " parity: " << parity;

                RESEND_SEND(true);
            }
            OP_code = 0; data1 = 0; data2 = 0; i = 0; parity = 0;
            executeCommand = false; OP_or_DATA = true;

        }
    }
}

void childReceive::RESEND_SEND(const bool bit)
{
    static QMutex mutex;

    mutex.lock();
    RESEND = bit;
    mutex.unlock();
}
