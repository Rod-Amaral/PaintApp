#include "MainWindow.h"

MainWindow::MainWindow(QWidget* const parent)
    : QWidget(parent),  brush(Qt::darkGreen, Qt::SolidPattern), pen(brush, 4, Qt::DotLine, Qt::RoundCap),
      Image(X_leng, Y_leng, QImage::Format_RGB32)
{
    ImageThread = new MainImage_Thread(this);
    BCP_SendThread = new mainSend(this);

    //Once finished wihth writing to Image, update window
    QObject::connect(ImageThread, &QThread::finished,
                     this, &MainWindow::ImagePaint_finished);

    setFixedSize(X_leng+100,Y_leng);
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
    static QMutex mut;

    //Paints Image on window
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    mut.lock();
    painter.drawImage(event->rect(), Image, event->rect());
    mut.unlock();

    //Side bar
    painter.setPen(QPen(Qt::black, 6, Qt::SolidLine, Qt::SquareCap));
    painter.drawLine(X_leng+2,0,X_leng+2,Y_leng);

    painter.end();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    static QPainter painter;
    static QPoint CurrentPoint;

    //Paints line on window and emits the same command for the other window
    if(ImageThread->isFinished())
    {
        CurrentPoint.setX(event->x());
        CurrentPoint.setY(event->y());
        PaintLine(CurrentPoint);
        SendPaintLine(CurrentPoint);
    }
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    static QPainter painter;
    static QPoint CurrentPoint;

    //Paints point on window and emits the same command for the other window
    if(ImageThread->isFinished() | LastPoint.isNull())
    {
        CurrentPoint.setX(event->x());
        CurrentPoint.setY(event->y());
        PaintPoint(CurrentPoint);
        SendPaintPoint(CurrentPoint);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    static QMutex mutex;
    static uint32_t color;
    static bool t = 0;

    //If R is pressed, clear Image and emit signal for other window to do the same
    if(event->key() == Qt::Key_R)
    {
        mutex.lock();
        Image.fill(Qt::white);

        //Clear Image OP code
        BCP_SendThread->setOP_code(2);
        mutex.unlock();
        BCP_SendThread->start();

        update();
    }
    else if(event->key() == Qt::Key_C)
    {
        if(t)
            color = 0xdd7788ff;
        else
            color = 0x0;
        t = !t;

        mutex.lock();
        brush.setColor(color);
        pen.setBrush(brush);
        mutex.unlock();

        while(BCP_SendThread->isRunning()){}
        BCP_SendThread->setOP_code(5);
        BCP_SendThread->setData1(color & 65535);
        BCP_SendThread->setData2((color>>16) & 65535);
        BCP_SendThread->start();
    }
    else if(event->key() == Qt::Key_V)
    {
        if(t)
        {
            setPen(Qt::Dense4Pattern, Qt::SolidLine, 8, Qt::RoundCap, Qt::SvgMiterJoin);
        }
        else
        {
            setPen(Qt::SolidPattern, Qt::DashLine, 20, Qt::SquareCap, Qt::RoundJoin);
        }
        t = !t;

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

    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(4);
    BCP_SendThread->setData1( ((unsigned int)bs) + ((unsigned int)(ps<<5)) + ((unsigned int)(penWidth<<8)) );
    BCP_SendThread->setData2( ((unsigned int)(pcs/0x10)) + ((unsigned int)( (pjs<0x100) ? (pjs/0x40) : 0b11 )<<2) );
    BCP_SendThread->start();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //Close Window OP code
    while(BCP_SendThread->isRunning()){}
    BCP_SendThread->setOP_code(3);
    BCP_SendThread->start();
    event->accept();
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

//I couldn't slot update directly -_-
void MainWindow::ImagePaint_finished()
{
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


