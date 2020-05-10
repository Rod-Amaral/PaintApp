#ifndef MAINTHREADS_H
#define MAINTHREADS_H

#include <QThread>
#include <QMutex>
#include <QPoint>

class MainWindow;

//Class used for multithreading in painting to Images
class MainImage_Thread : public QThread
{
public:
    explicit MainImage_Thread(MainWindow* const window);

    void run() override;

    void setPoint(const QPoint & p);
    void setToggle(const bool t);

    //Used to toggle between drawing a line or a point
    static const bool LINE = true;
    static const bool POINT = false;

private:
    MainWindow* Window;
    QPoint point;
    bool Line_Point; //Used to select between Line/Point Drawing

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
    bool SendMessage;
    uint8_t OP_code;
    int16_t data1;
    int16_t data2;
};

#endif // TMAINTHREADS_H
