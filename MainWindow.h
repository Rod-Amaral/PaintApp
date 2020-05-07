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

public:
    void SendPaintPoint(QPoint CurrentPoint);
    void SendPaintLine(QPoint CurrentPoint);
    void SendClearImage();
    void SendCloseWindow();

signals:
    void SEND_BIT(bool bit);
};

#endif // MAINWINDOW_H
