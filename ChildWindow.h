#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

#include <vector>

#include <QBrush>
#include <QPen>
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>


//Child window class
class ChildWindow : public QWidget
{
	Q_OBJECT

public:
    ChildWindow(QWidget *parent = 0);
    ~ChildWindow();

	static QPen pen; //Pen Style

	static const uint16_t X_leng = 700;
	static const uint16_t Y_leng = 800;
	//static uint16_t X_menu = 100;

	//Not made static, for signal and slots mechanism
	int x, y;

	QPainter painter;

	//Map of painted pixels
    std::vector<std::vector<bool>> PixelMap;

protected:
	void paintEvent(QPaintEvent *event);

public slots:
	void setPoint(int x, int y);

signals:
	void pointChanged(int x, int y);
};

#endif
