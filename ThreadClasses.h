#ifndef THREADCLASSES_H
#define THREADCLASSES_H

#include <QThread>
#include <QMutex>
#include <QPoint>


class ChildWindow; class MainWindow;

//Class used for multithreading in painting to Images
class PaintImage_Thread : public QThread
{
public:
    explicit PaintImage_Thread(ChildWindow* const window);

    void run() override;

    void setPoint(const QPoint & p);
    void setToggle(const bool t);

    //Used to toggle between drawing a line or a point
    static const bool LINE = true;
    static const bool POINT = false;

private:
    ChildWindow* Window;
    QPoint point;
    bool Line_Point; //Used to select between Line/Point Drawing

};


//Class used for multihreading in receiving data for the BCP
class childReceive : public QThread
{
public:
    explicit childReceive(ChildWindow* const window);

    void run() override;

    void setBIT(const bool b);

private:
    ChildWindow* Window;
    bool bit;
};


//Class used for multihreading in sending data for the BCP
class mainSend : public QThread
{
public:
    explicit mainSend(MainWindow* const window);

    void run() override;

    void setOP_code(const uint8_t op);
    void setData1(const int16_t data);
    void setData2(const int16_t data);

private:
    MainWindow* Window;
    uint8_t OP_code;
    int16_t data1;
    int16_t data2;
};

#endif // THREADCLASSES_H
