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
    Window->update();
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

bool ParityCalculation(const uint8_t OP_code, const uint16_t data1, const uint16_t data2)
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

void mainSend::run()
{
    static QMutex mutex;

    do
    {
        for(size_t i = 0; i<4; i++)
            SEND_BIT( (OP_code>>i) & 1 );

        SEND_BIT(ParityCalculation(OP_code,data1,data2)); //parity

        if( (OP_code>1 && OP_code <4) || (OP_code == 7) )
            SEND_BIT(0);
        else
        {
            for(size_t i = 0; i<16; i++)
                SEND_BIT( (data1>>i) & 1 );
            for(size_t i = 0; i<16; i++)
                SEND_BIT( (data2>>i) & 1 );
        }
        old_OP_code = current_OP_code; old_data1 = current_data1; old_data2 = current_data2;
        current_OP_code = OP_code; current_data1 = data1; current_data2 = data2;
        data1 = 0; data2 = 0;

        if(RESEND)
            Resend();

    } while(RESEND);
}

void mainSend::SEND_BIT(bool bit)
{
    static QMutex mutex;

    mutex.lock();
    BIT = bit;
    DO_READ = true;
    mutex.unlock();

    while(DO_READ){}
}

void mainSend::Resend()
{
    static QMutex mutex;

    mutex.lock();
    uint8_t hold_OP_code(current_OP_code);
    uint16_t hold_data1(current_data1), hold_data2(current_data2);

    setOP_code(old_OP_code);
    setData1(old_data1);
    setData2(old_data2);
    mutex.unlock();

    do
    {
        for(size_t i = 0; i<4; i++)
            SEND_BIT( (OP_code>>i) & 1 );

        SEND_BIT(ParityCalculation(OP_code,data1,data2)); //parity

        if( (OP_code>1 && OP_code <4) || (OP_code == 7) )
            SEND_BIT(0);
        else
        {
            for(size_t i = 0; i<16; i++)
                SEND_BIT( (data1>>i) & 1 );
            for(size_t i = 0; i<16; i++)
                SEND_BIT( (data2>>i) & 1 );
        }
        old_OP_code = current_OP_code; old_data1 = current_data1; old_data2 = current_data2;
        current_OP_code = OP_code; current_data1 = data1; current_data2 = data2;
        data1 = 0; data2 = 0;

        if(RESEND)
            Resend();

    } while(RESEND);

    mutex.lock();
    setOP_code(hold_OP_code);
    setData1(hold_data1);
    setData2(hold_data2);
    mutex.unlock();

    do
    {
        for(size_t i = 0; i<4; i++)
            SEND_BIT( (OP_code>>i) & 1 );

        SEND_BIT(ParityCalculation(OP_code,data1,data2)); //parity

        if( (OP_code>1 && OP_code <4) || (OP_code == 7) )
            SEND_BIT(0);
        else
        {
            for(size_t i = 0; i<16; i++)
                SEND_BIT( (data1>>i) & 1 );
            for(size_t i = 0; i<16; i++)
                SEND_BIT( (data2>>i) & 1 );
        }
        old_OP_code = current_OP_code; old_data1 = current_data1; old_data2 = current_data2;
        current_OP_code = OP_code; current_data1 = data1; current_data2 = data2;
        data1 = 0; data2 = 0;

        if(RESEND)
            Resend();

    } while(RESEND);
}

void mainSend::setOP_code(const uint8_t op)
{
    static QMutex mutex;
    mutex.lock();
    OP_code = op;
    mutex.unlock();
}

void mainSend::setData1(const int16_t data)
{
    static QMutex mutex;
    mutex.lock();
    data1 = data;
    mutex.unlock();
}

void mainSend::setData2(const int16_t data)
{
    static QMutex mutex;
    mutex.lock();
    data2 = data;
    mutex.unlock();
}

/*
 OP Codes
 0: PaintPoint
 1: PaintLine
 2: ClearWindow
 3: ExitProgram
 4: Change Pen/Brush
 5: Change Color
 6: Resize
 7: FullsScreen
 8: LastPoint
 9: Sync
 10: skipped position, for Sync
*/

