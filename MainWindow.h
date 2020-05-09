#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ChildWindow.h"

//Main Window class
class MainWindow : public ChildWindow
{
    Q_OBJECT

public:
    MainWindow(ChildWindow* const parent = 0);
    ~MainWindow();

    //Worker thread for sending data with the BCP
    mainSend* BCP_SendThread;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

public:
    void SendPaintPoint(const QPoint & CurrentPoint);
    void SendPaintLine(const QPoint & CurrentPoint);
    void SendClearImage();
    void SendCloseWindow();

signals:
    void SEND_BIT(const bool bit);
};

#endif // MAINWINDOW_H
