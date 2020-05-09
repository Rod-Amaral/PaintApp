#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPen>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include "MainThreads.h"

//Main Window class
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget* const parent = 0);
    ~MainWindow();

    //Holds pen details, used in painting
    QBrush brush;
    QPen pen;

    //Window size, for now
    const uint16_t X_leng = 700;
    const uint16_t Y_leng = 800;

    //Holds last recorded point for drawing a line
    QPoint LastPoint;

    //Where the Image is held
    QImage Image;

    //Worker thread for Image painting,
    MainImage_Thread* ImageThread;

    //Worker thread for sending data with the BCP
    mainSend* BCP_SendThread;

    void PaintPoint(const QPoint & point);
    void PaintLine(const QPoint & point);

    void ClearImage();
    void ImagePaint_finished();

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
