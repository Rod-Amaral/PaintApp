#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include <QWidget>
#include <QPushButton>
#include <vector>
#include "MainWindow.h"

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


class SettingsMenu : public QWidget
{
    Q_OBJECT
public:
    SettingsMenu(QWidget* const parent = 0);
    ~SettingsMenu();

    void PenSelection();

     PushButton* Buttons;

public slots:
    void toggleWindow(const int x, const int y);
    void chooseColour(PushButton* pointer);

signals:
    void sendColour(QRgb colour);
};

#endif // SETTINGSMENU_H
