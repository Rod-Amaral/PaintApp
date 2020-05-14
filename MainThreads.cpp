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
    : Window(window), OP_code(0), data1(0), data2(0), previous_OP(0), current_OP(0),
      previous_data1(0), current_data1(0), previous_data2(0), current_data2(0)
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
    //Send OP_code
    emit Window->SEND_BIT((bool)(OP_code & 1));
    emit Window->SEND_BIT((bool)(OP_code & 2));
    emit Window->SEND_BIT((bool)(OP_code & 4));
    emit Window->SEND_BIT((bool)(OP_code & 8));

    //Parity bit
    emit Window->SEND_BIT(ParityCalculation(OP_code,data1,data2));

    //Send data
    if((OP_code>1 && OP_code <4) || (OP_code == 7))
        emit Window->SEND_BIT(0);
    else
    {
        for(size_t i = 0; i<16; i++)
            emit Window->SEND_BIT((bool)1&(data1 >> i));
        for(size_t i = 0; i<16; i++)
            emit Window->SEND_BIT((bool)1&(data2 >> i));
    }

    //Hold last two commands and data, in case of parity fail, to resend them
    previous_data1 = current_data1; previous_data2 = current_data2; previous_OP = current_OP;
    current_data1 = data1; current_data2 = data2; current_OP = OP_code;
    data1 = 0; data2 = 0;
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

//Resends previous two commands
void mainSend::Resend()
{
    static QMutex mutex;
    mutex.lock();
    uint8_t holdCurrent_OP = current_OP;
    int16_t holdCurrent_data1 = current_data1;
    int16_t holdCurrent_data2 = current_data2;

    setOP_code(previous_OP);
    setData1(previous_data1);
    setData2(previous_data2);
    mutex.unlock();
    start();

    while(isRunning()){}
    mutex.lock();
    setOP_code(holdCurrent_OP);
    setData1(holdCurrent_data1);
    setData2(holdCurrent_data2);
    mutex.unlock();
    start();
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
*/

