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

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void doPaintPoint(QPoint CurrentPoint);
    void doPaintLine(QPoint CurrentPoint);
    void doclearImage();
    void doCloseWindow();
};

#endif // MAINWINDOW_H
