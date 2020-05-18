#include "MainWindow.h"

MainWindow::MainWindow(QWidget* const parent)
    : QWidget(parent), initiate(true), stop(true), brush(Qt::darkGreen, Qt::SolidPattern),
      pen(brush, 8, Qt::SolidLine, Qt::RoundCap), Image(X_leng, Y_leng, QImage::Format_RGB32)
{
    resize(X_leng,Y_leng);
    ImageThread = new MainImage_Thread(this);
    BCP_SendThread = new mainSend(this);

    Image.fill(Qt::white);
}

MainWindow::~MainWindow()
{
    ImageThread->deleteLater();
    BCP_SendThread->deleteLater();
    delete BCP_SendThread;
    delete ImageThread;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    static QPainter painter;
    static QMutex mutex;

    //Paints Image on window
    mutex.lock();
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(event->rect(), Image, event->rect());
    painter.end();
    mutex.unlock();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    static QPoint CurrentPoint;

    //If sync is on, don't take in commands
    if(stop)
    {
        if(event->x()<=width() && event->y()<=height())
        {
            //Paints line on window and emits the same command for the other window
            if(ImageThread->isFinished() && wasLeftButton)
            {
                CurrentPoint.setX(event->x());
                CurrentPoint.setY(event->y());
                PaintLine(CurrentPoint);
                SendPaintLine(CurrentPoint);
            }
        }
        else
        {
            LastPoint = QPoint(event->x(),event->y());
            //Set Last Point OP_code, to prevent a bug when moving out of Window boundries
            BCP_SendThread->setOP_code(8);
            BCP_SendThread->setData1(event->x());
            BCP_SendThread->setData2(event->y());
            BCP_SendThread->start();

        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    static QPoint CurrentPoint;

     //If sync is on, don't take in commands
    if(stop)
    {
        if((event->button()==Qt::LeftButton))
        {
            //Paints point on window and emits the same command for the other window
            if((ImageThread->isFinished() || LastPoint.isNull()))
            {
                wasLeftButton = true;
                CurrentPoint.setX(event->x());
                CurrentPoint.setY(event->y());
                PaintPoint(CurrentPoint);
                SendPaintPoint(CurrentPoint);
            }
            emit closeSettingsWindow();
        }
        else if((event->button()==Qt::RightButton)) //Toggle settings menu
            emit toggleSettingsWindow(event->globalX(), event->globalY());
    }
}

//This is mostly used to prevent a bug when moving out of the window
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if((event->button()==Qt::LeftButton))
        wasLeftButton = false;
}

static bool doClose(false); // Used to implement Alt + f4
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    static QMutex mutex;

    //If R is pressed, clear Image and emit signal for other window to do the same
    if((event->key() == Qt::Key_R) && stop)
    {
        mutex.lock();
        Image.fill(Qt::white);
        mutex.unlock();

        //Clear Image OP code
        while(BCP_SendThread->isRunning()){}
        BCP_SendThread->setOP_code(2);
        BCP_SendThread->start();

        update();
    }
    //Alt + f4 to close window
    else if(event->key() == Qt::Key_Alt)
        doClose = true;
    //f11 for FullScreen
    else if(event->key() == Qt::Key_F11 && stop)
    {
        if(isFullScreen())
            showNormal();
        else
            showFullScreen();
    }
    if((event->key() == Qt::Key_F4) && doClose)
    {
        close();
        doClose = false;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    //If Alt released, don't allow ALt+f4
    if(event->key() == Qt::Key_Alt)
        doClose = false;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    static QMutex mutex;

    //If sync is on, don't take in commands
   if(stop)
   {
        mutex.lock();
        if( (Image.width()<width()) || (Image.height()<height()) )
        {
            int16_t oldX = event->oldSize().width();
            int16_t oldY = event->oldSize().height();

            //Resized Image baground is black
            Image = Image.copy(0,0,width(),height());

            //So we need to paint those areas
            static QPainter painter;
            painter.begin(&Image);
            painter.fillRect(0,oldY,width(),(height()-oldY),Qt::white);
            painter.fillRect(oldX,0,(width()-oldX),oldY,Qt::white);
            painter.end();
        }
        mutex.unlock();

        while(BCP_SendThread->isRunning()){}
        if(isFullScreen())
            BCP_SendThread->setOP_code(7); //Max window
        else
        {
            //Resize Window OP_code
            BCP_SendThread->setOP_code(6);
            BCP_SendThread->setData1(event->size().width());
            BCP_SendThread->setData2(event->size().height());
        }
        BCP_SendThread->start();
   }
   else
       event->ignore();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //Close Window OP code
    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(3);
    BCP_SendThread->start();
    event->accept();
}

void MainWindow::set()
{
    //Set Pen
    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(4);
    BCP_SendThread->setData1( ((uint8_t)brush.style()) + ((uint8_t)(pen.style()<<5)) + ((uint32_t)(pen.width()<<8)) );
    BCP_SendThread->setData2( ((uint8_t)(pen.capStyle()/0x10)) + ((uint8_t)( (pen.joinStyle()<0x100) ? (pen.joinStyle()/0x40) : 0b11 )<<2) );
    BCP_SendThread->start();

    update();
}

void MainWindow::SyncImages()
{
    static QMutex mutex;
    static uint16_t i(0), j(0);
    static bool skiped(false);

    while(!stop)
    {
        //Makes reveiver thread ready to take pixel data
        if(initiate)
        {
            while(BCP_SendThread->isRunning()){}
            BCP_SendThread->setOP_code(9);
            mutex.lock();
            BCP_SendThread->setData1(Image.width());
            BCP_SendThread->setData2(Image.height());
            mutex.unlock();
            BCP_SendThread->start();
        }

        //Starts sending pixel data
        if(!initiate)
        {
            //Keep going through pixels, until finding one that isn't white
            while(Image.pixel(i,j) == 4294967295)
            {
                if(i<Image.width())
                    i++;
                if(i==Image.width())
                {
                    j++;
                    i = 0;
                }
                skiped = true;
                if(j == Image.height())
                {
                    i = 0;
                    j = Image.height();
                    break;
                }
            }

            if(skiped)
            {
                //Send new position
                while(BCP_SendThread->isRunning()){}
                BCP_SendThread->setOP_code(10);
                BCP_SendThread->setData1(i);
                BCP_SendThread->setData2(j);
                BCP_SendThread->start();
                skiped = false;
            }
            else
            {
                //Send pixel data
                while(BCP_SendThread->isRunning()){}
                BCP_SendThread->setOP_code(9);
                BCP_SendThread->setData1(Image.pixel(i,j) & 0xFFFF);
                BCP_SendThread->setData2((Image.pixel(i,j)>>16) & (0xFFFF));
                BCP_SendThread->start();

                //Goes one pixel ahed
                if(i<Image.width())
                    i++;
                if(i==Image.width())
                {
                    j++;
                    i = 0;
                }
                if(j == Image.height())
                {
                    i = 0;
                    j = Image.height();
                }
            }
        }
        if(initiate)
            initiate = false;
        if(j == Image.height())
        {
            initiate = true;
            i = 0; j = 0; skiped = false;
            stop = true;
        }
    }
}

void MainWindow::setPen(Qt::BrushStyle bs, Qt::PenStyle ps, uint8_t penWidth, Qt::PenCapStyle pcs, Qt::PenJoinStyle pjs)
{
    static QMutex mutex;

    mutex.lock();
    brush.setStyle(bs);
    pen.setBrush(brush);
    pen.setStyle(ps);
    pen.setWidth(penWidth);
    pen.setCapStyle(pcs);
    pen.setJoinStyle(pjs);
    mutex.unlock();

    //Pen Settings OP code
    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(4);
    BCP_SendThread->setData1( ((unsigned int)bs) + ((unsigned int)(ps<<5)) + ((unsigned int)(penWidth<<8)) );
    BCP_SendThread->setData2( ((unsigned int)(pcs/0x10)) + ((unsigned int)( (pjs<0x100) ? (pjs/0x40) : 0b11 )<<2) );
    BCP_SendThread->start();
}

void MainWindow::setPenSize(uint8_t penWidth)
{
    static QMutex mutex;

    mutex.lock();
    pen.setWidth(penWidth);
    mutex.unlock();

    //Pen Settings OP code
    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(4);
    BCP_SendThread->setData1( ((unsigned int)brush.style()) + ((unsigned int)(pen.style()<<5)) + ((unsigned int)(penWidth<<8)) );
    BCP_SendThread->setData2( ((unsigned int)(pen.capStyle()/0x10)) + ((unsigned int)( (pen.joinStyle()<0x100) ? (pen.joinStyle()/0x40) : 0b11 )<<2) );
    BCP_SendThread->start();
}

void MainWindow::setBrushStyle(Qt::BrushStyle bs)
{
    static QMutex mutex;
qDebug() << bs;
    mutex.lock();
    brush.setStyle(bs);
    pen.setBrush(brush);
    mutex.unlock();

    //Pen Settings OP code
    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(4);
    BCP_SendThread->setData1( ((unsigned int)brush.style()) + ((unsigned int)(pen.style()<<5)) + ((unsigned int)(pen.width()<<8)) );
    BCP_SendThread->setData2( ((unsigned int)(pen.capStyle()/0x10)) + ((unsigned int)( (pen.joinStyle()<0x100) ? (pen.joinStyle()/0x40) : 0b11 )<<2) );
    BCP_SendThread->start();
}

void MainWindow::setPenStyle(Qt::PenStyle ps)
{
    static QMutex mutex;

    mutex.lock();
    pen.setStyle(ps);
    mutex.unlock();

    //Pen Settings OP code
    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(4);
    BCP_SendThread->setData1( ((unsigned int)brush.style()) + ((unsigned int)(pen.style()<<5)) + ((unsigned int)(pen.width()<<8)) );
    BCP_SendThread->setData2( ((unsigned int)(pen.capStyle()/0x10)) + ((unsigned int)( (pen.joinStyle()<0x100) ? (pen.joinStyle()/0x40) : 0b11 )<<2) );
    BCP_SendThread->start();
}
void MainWindow::setPenCapStyle(Qt::PenCapStyle pcs)
{
    static QMutex mutex;

    mutex.lock();
    pen.setCapStyle(pcs);
    mutex.unlock();

    //Pen Settings OP code
    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(4);
    BCP_SendThread->setData1( ((unsigned int)brush.style()) + ((unsigned int)(pen.style()<<5)) + ((unsigned int)(pen.width()<<8)) );
    BCP_SendThread->setData2( ((unsigned int)(pen.capStyle()/0x10)) + ((unsigned int)( (pen.joinStyle()<0x100) ? (pen.joinStyle()/0x40) : 0b11 )<<2) );
    BCP_SendThread->start();
}

void MainWindow::setPenJoinStyle(Qt::PenJoinStyle pjs)
{
    static QMutex mutex;

    mutex.lock();
    pen.setJoinStyle(pjs);
    mutex.unlock();

    //Pen Settings OP code
    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(4);
    BCP_SendThread->setData1( ((unsigned int)brush.style()) + ((unsigned int)(pen.style()<<5)) + ((unsigned int)(pen.width()<<8)) );
    BCP_SendThread->setData2( ((unsigned int)(pen.capStyle()/0x10)) + ((unsigned int)( (pen.joinStyle()<0x100) ? (pen.joinStyle()/0x40) : 0b11 )<<2) );
    BCP_SendThread->start();
}

void MainWindow::setColour(QRgb color)
{
    static QMutex mutex;
    mutex.lock();
    brush.setColor(color);
    pen.setBrush(brush);
    mutex.unlock();

    //Set Colour OP code
    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(5);
    BCP_SendThread->setData1((color & 65535));
    BCP_SendThread->setData2((color>>16) & 65535);
    BCP_SendThread->start();
}

void MainWindow::PaintPoint(const QPoint & point)
{
    while(ImageThread->isRunning()){}
    ImageThread->setPoint(point);
    ImageThread->setToggle(MainImage_Thread::POINT);
    ImageThread->start();
}

void MainWindow::PaintLine(const QPoint & point)
{
    while(ImageThread->isRunning()){}
    ImageThread->setPoint(point);
    ImageThread->setToggle(MainImage_Thread::LINE);
    ImageThread->start();
}

void MainWindow::ClearImage()
{
    static QMutex mutex;

    //Clears Image, by making it all white and calls window PaintEvent
    mutex.lock();
    Image.fill(Qt::white);
    mutex.unlock();
    update();
}

void MainWindow::SendPaintPoint(const QPoint & CurrentPoint)
{
    //Send Point OP code
    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(0);
    BCP_SendThread->setData1(CurrentPoint.x());
    BCP_SendThread->setData2(CurrentPoint.y());
    BCP_SendThread->start();
}

void MainWindow::SendPaintLine(const QPoint & CurrentPoint)
{
    //Send Line OP code
    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(1);
    BCP_SendThread->setData1(CurrentPoint.x());
    BCP_SendThread->setData2(CurrentPoint.y());
    BCP_SendThread->start();
}
