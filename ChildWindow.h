#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

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

	//Not made static, for signal and slots mechanism
    QPoint LastPoint;

    //Where the Image is held
    QImage Image;

protected:
	void paintEvent(QPaintEvent *event);

public slots:
    void PaintPoint(QPoint point);
    void PaintLine(QPoint point);

signals:
    void pointChanged(QPoint point);
};

#endif
