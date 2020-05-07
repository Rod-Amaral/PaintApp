#ifndef THREADCLASSES_H
#define THREADCLASSES_H

#include <QThread>
#include <QMutex>
#include <QPoint>


class ChildWindow;
class PaintImage_Thread : public QThread
{
public:
    explicit PaintImage_Thread(ChildWindow* window);

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

class childReceive : public QThread
{
    explicit childReceive();

    void run() override;
};

class mainSend : public QThread
{
    explicit mainSend();

    void run() override;
};

#endif // THREADCLASSES_H
