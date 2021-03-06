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

    //Used to initiate A Sync command, and prevent the rest of the program from running
    bool initiate;
    //Stops Syncing
    bool stop;

    //Used to know if the left button was pressed
    bool wasLeftButton;

    //Holds pen details, used in painting
    QBrush brush;
    QPen pen;

    //Default Window size
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
    void setPen(Qt::BrushStyle bs, Qt::PenStyle ps, uint8_t penWidth, Qt::PenCapStyle pcs, Qt::PenJoinStyle pjs);
    void set();
    void ClearImage();
    void SyncImages();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

public:
    void SendPaintPoint(const QPoint & CurrentPoint);
    void SendPaintLine(const QPoint & CurrentPoint);
    void SendClearImage();
    void SendCloseWindow();

public slots:
    void setColour(uint32_t color);
    void setPenSize(uint8_t penWidth);
    void setBrushStyle(Qt::BrushStyle bs);
    void setPenStyle(Qt::PenStyle ps);
    void setPenCapStyle(Qt::PenCapStyle pcs);
    void setPenJoinStyle(Qt::PenJoinStyle pjs);

signals:
    void toggleSettingsWindow(const int x, const int y);
    void closeSettingsWindow();
};

#endif // MAINWINDOW_H
