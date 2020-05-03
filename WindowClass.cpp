#include "WindowClass.h"
#include <QTextStream>

static QTextStream out(stderr); //Debug

//Static variables
bool childWindow::toggle(0);
QPen childWindow::pen(Qt::darkGreen, 10);

childWindow::childWindow(QWidget *parent) 
	: QWidget(parent, Qt::Window)
{
	setFixedSize(700,800);
}

childWindow::~childWindow()
{}

void childWindow::paintEvent(QPaintEvent *event)
{
	//painter.begin(this) begins painting
	if(toggle)
	{
		painter.begin(this);
		painter.setPen(pen);
		painter.drawPoint(point);
		painter.end();
	}
	/*
	painter.end() is necessary! to release resources used in painting.
	Otherwise we would need to call the deconstructor 
	Given code in p6 was, shit.
	It had to make a new QLabel, QPen and QFont for each paint event, and then decosntruct them lol 
	*/
}

void childWindow::setQPoint(QPoint q)
{
	if(q != point)
	{
		point = q;
		update();
		emit pointChanged(q);
	}
}



MahWindow::MahWindow(childWindow *parent) : childWindow(parent)
{	
	setFixedSize(700,800);
}

MahWindow::~MahWindow()
{}

void MahWindow::paintEvent(QPaintEvent *event)
{
	if(toggle)
	{
		painter.begin(this);
		painter.setPen(pen);
		painter.drawPoint(point);
		painter.end();
	}
}

void MahWindow::mouseMoveEvent(QMouseEvent *event)
{
	setQPoint( QPoint(event->x(),event->y()) );
}
void MahWindow::mousePressEvent(QMouseEvent *event)
{
	if(!toggle)
		toggle = 1;
	setQPoint( QPoint (event->x(),event->y()) );
}

