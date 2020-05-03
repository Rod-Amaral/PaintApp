#ifndef WINDOW_H
#define WINDOW_H

//#include <map> look into this!
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
	static bool toggle; //Used to only paint, after mouse is pressed once

	//Not made static, for signal and slots mechanism
	QPoint point;
	//Not made static, for multithread implementation
	QPainter painter;
	
protected:
	void paintEvent(QPaintEvent *event);

public slots:
	void setQPoint(QPoint p);

signals:
	void pointChanged(QPoint p);
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