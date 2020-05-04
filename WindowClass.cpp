#include "WindowClass.h"
#include <QTextStream>

static QTextStream out(stderr); //Debug

static const uint8_t Pen_size = 5;
static const QBrush Brush = Qt::darkGreen; 

//Static variables
QPen childWindow::pen(Brush, Pen_size);

childWindow::childWindow(QWidget *parent) 
	: QWidget(parent, Qt::Window), PixelMap(X_leng * Y_leng)
{
	setFixedSize(X_leng,Y_leng);
}

childWindow::~childWindow()
{}

void childWindow::paintEvent(QPaintEvent *event)
{
	painter.begin(this);
	painter.setPen(pen);
	painter.drawPoints(PixelMap);
	painter.end();
}

void childWindow::setPoint(int x, int y)
{
	PixelMap << QPoint(x,y);
	update();
	emit pointChanged(x,y);
}



MahWindow::MahWindow(childWindow *parent) : childWindow(parent)
{	
	setFixedSize(X_leng,Y_leng);
	painter.setPen(pen);
}

MahWindow::~MahWindow()
{}


void MahWindow::paintEvent(QPaintEvent *event)
{
	painter.begin(this);
	painter.setPen(pen);
	painter.drawPoints(PixelMap);
	painter.end();
}

void MahWindow::mouseMoveEvent(QMouseEvent *event)
{
	if( (event->x()<X_leng && event->y()<Y_leng && event->x()>0 && event->y()>0) )
		setPoint( event->x(),event->y() );
}
void MahWindow::mousePressEvent(QMouseEvent *event)
{
	if( (event->x()<X_leng && event->y()<Y_leng && event->x()>0 && event->y()>0) )
		setPoint( event->x(),event->y() );
	//&& (!PixelMap.containsPoint(QPoint(event->x(),event->y()), Qt::OddEvenFill))
}

