#include "ChildWindow.h"
#include <QTextStream>

static QTextStream out(stderr); //Debug

static const uint8_t Pen_size = 10;
static const QBrush Brush = Qt::darkGreen; 

//Static variables
QPen ChildWindow::pen(Brush, Pen_size);

ChildWindow::ChildWindow(QWidget *parent)
	: QWidget(parent, Qt::Window), PixelMap(Y_leng, std::vector<bool>(X_leng))
{
	setFixedSize(X_leng,Y_leng);
}

ChildWindow::~ChildWindow()
{}

void ChildWindow::paintEvent(QPaintEvent *event)
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

void ChildWindow::setPoint(int x, int y)
{
	PixelMap[y][x] = true;
	emit pointChanged(x,y);
}





