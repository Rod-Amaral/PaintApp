#include "WindowClass.h"
#include <QTextStream>

static QTextStream out(stderr); //Debug

static const uint8_t Pen_size = 5;
static const QBrush Brush = Qt::darkGreen; 

//Static variables
QPen childWindow::pen(Brush, Pen_size);

childWindow::childWindow(QWidget *parent) 
	: QWidget(parent, Qt::Window), PixelMap(Y_leng, std::vector<bool>(X_leng))
{
	setFixedSize(X_leng,Y_leng);
}

childWindow::~childWindow()
{}

void childWindow::paintEvent(QPaintEvent *event)
{
	static uint16_t i,j;

	//painter.begin(this) begins painting
	for(j=0; j<Y_leng; j++)
	{
		for(i=0; i<X_leng; i++)
			if(PixelMap[j][i])
			{
				painter.begin(this);
				painter.setPen(pen);
				painter.drawPoint(i,j);
				painter.end();
			}
	}
	//painter.end() is necessary! to release resources used in painting.
}

void childWindow::setPoint(int x, int y)
{
	if( (x<X_leng && y<Y_leng && x>0 && y>0) && (!PixelMap[y][x]) )
	{
		//out << "Coords( " << x << ',' << y << " )\n";
		PixelMap[y][x] = true;
		update();
		emit pointChanged(x,y);
	}
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
	static uint16_t i,j;
	for(j=0; j<Y_leng; j++)
	{
		for(i=0; i<X_leng; i++)
			if(PixelMap[j][i])
			{
				painter.begin(this);
				painter.setPen(pen);
				painter.drawPoint(i,j);
				painter.end();
			}
	}
}

void MahWindow::mouseMoveEvent(QMouseEvent *event)
{
	setPoint( event->x(),event->y() );
}
void MahWindow::mousePressEvent(QMouseEvent *event)
{
	setPoint( event->x(),event->y() ) ;
}

