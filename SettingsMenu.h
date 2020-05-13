#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include "MainWindow.h"

//This class exists to send a click() signal, with the pointer of the button as data
class PushButton : public QPushButton
{
    Q_OBJECT

public:
    PushButton(QWidget* const parent = 0);

public slots:
    void wasClicked();

signals:
    void sendPointer(PushButton* pointer);
};


//Settings menu class
class SettingsMenu : public QWidget
{
    Q_OBJECT
public:
    SettingsMenu(QWidget* const parent = 0);
    ~SettingsMenu();

     PushButton* Buttons;
     QSpinBox penSizeBox;
     PushButton* penSetSizeButton;

     void set();
     void askPenSize();

protected:
     void showEvent(QShowEvent *event) override;
     void keyPressEvent(QKeyEvent *event) override;
     void mousePressEvent(QMouseEvent *event) override;

public slots:
    void toggleWindow(const int x, const int y);
    void chooseColour(PushButton* pointer);
    void choosePenSize(PushButton* pointer);
    void chooseSettings(PushButton* pointer);
    void checkPenSize();

signals:
    void sendColour(QRgb colour);
    void sendPenSize(uint8_t size);
};

#endif // SETTINGSMENU_H
