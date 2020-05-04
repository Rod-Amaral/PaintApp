#ifndef WINDOW_H
#define WINDOW_H

//#include <map> look into this!
#include <vector>

#include <QBrush>
#include <QPen>
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>


//Child window class
class childWindow : public QWidget
{
	Q_OBJECT

public:
	childWindow(QWidget *parent = 0);
	~childWindow();

	static QPen pen; //Pen Style

	static const uint16_t X_leng = 700;
	static const uint16_t Y_leng = 800;
	//static uint16_t X_menu = 100;

	//Not made static, for signal and slots mechanism
	int x, y;

	QPainter painter;

	//Map of painted pixels
	QPolygon PixelMap;

protected:
	void paintEvent(QPaintEvent *event);

public slots:
	void setPoint(int x, int y);

signals:
	void pointChanged(int x, int y);
};


//Main Window class
class MahWindow : public childWindow
{
	Q_OBJECT

public:
	MahWindow(childWindow *parent = 0);
	~MahWindow();

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
};

#endif