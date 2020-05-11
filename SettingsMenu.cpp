#include "SettingsMenu.h"
#include <QDebug>

SettingsMenu::SettingsMenu(QWidget* const parent) : QWidget(parent, Qt::Window)
{
    setFixedSize(300,600);
}

void SettingsMenu::toggleWindow(const int x, const int y)
{
    if(this->isHidden())
    {
        show();
        move(x,y);
    }
    else
        hide();
}
