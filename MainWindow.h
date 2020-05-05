#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ChildWindow.h"

//Main Window class
class MainWindow : public ChildWindow
{
    Q_OBJECT

public:
    MainWindow(ChildWindow *parent = 0);
    ~MainWindow();

    bool no_paint;

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
