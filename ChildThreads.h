#ifndef CHILDTHREADS_H
#define CHILDTHREADS_H

#include <QThread>
#include <QWidget>
#include <QMutex>
#include <QPoint>
#include <stdint.h>

extern bool BIT;
extern bool DO_READ;

extern bool RESEND;

class ChildWindow;

//Class used for multithreading in painting to Images
class ChildImage_Thread : public QThread
{
public:
    explicit ChildImage_Thread(ChildWindow* const window);

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

    void RESEND_SEND(const bool bit);

private:
    ChildWindow* Window;
};


#endif // CHILDTHREADS_H
