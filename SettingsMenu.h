#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QColorDialog>
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
     QColorDialog setColorScreen;

     PushButton* PenSettings_buttons;
     PushButton* BrushStyle_buttons;
     PushButton* PenStyle_buttons;
     PushButton* PenCap_buttons;
     PushButton* PenJoin_buttons;

     void set();
     void askPenSize();
     void askCustomColour();
     void askPenSettings();
     void askBrushStyle();
     void askPenStyle();
     void askPenCapStyle();
     void askPenJoinStyle();
     void askSync();

     void setPenButtons(PushButton* const & buttons, uint8_t amount, uint16_t extraSize);
     void setPenSettings_names(PushButton*const & buttons);
     void setBrushStyle_names(PushButton* const & buttons);
     void setPenStyle_names(PushButton* const & buttons);
     void setPenCapStyle_names(PushButton* const & buttons);
     void setPenJoinStyle_names(PushButton* const & buttons);

protected:
     void showEvent(QShowEvent *event) override;
     void keyPressEvent(QKeyEvent *event) override;
     void mousePressEvent(QMouseEvent *event) override;

public slots:
    void toggleWindow(const int x, const int y);

    void chooseColour(PushButton* pointer);
    void chooseQColour(QColor colour);
    void choosePenSize(PushButton* pointer);
    void chooseSettings(PushButton* pointer);
    void chooseBrushStyle(PushButton* pointer);
    void choosePenStyle(PushButton* pointer);
    void choosePenCapStyle(PushButton* pointer);
    void choosePenJoinStyle(PushButton* pointer);

    void checkPenSize();

signals:
    void sendColour(QRgb colour);
    void sendPenSize(uint8_t size);
    void sendBrushStyle(Qt::BrushStyle bs);
    void sendPenStyle(Qt::PenStyle ps);
    void sendPenCapStyle(Qt::PenCapStyle);
    void sendPenJoinStyle(Qt::PenJoinStyle);
};

#endif // SETTINGSMENU_H
