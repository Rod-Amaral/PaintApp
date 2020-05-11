#include "SettingsMenu.h"
#include <QDebug>

static const uint8_t but_n = 18;

PushButton::PushButton(QWidget* const parent) : QPushButton(parent)
{
    QObject::connect(this, &PushButton::clicked,
                     this, &PushButton::wasClicked);
}

void PushButton::wasClicked()
{
    emit sendPointer(this);
}

SettingsMenu::SettingsMenu(QWidget* const parent) : QWidget(parent, Qt::Window), Buttons(new PushButton[but_n])
{
    for(size_t i = 0; i<but_n; i++)
    {
        Buttons[i].setParent(this);
        QObject::connect(&(Buttons[i]), &PushButton::sendPointer,
                         this, &SettingsMenu::chooseColour);
    }

    const uint16_t X_leng(120);
    const uint16_t Y_leng(((but_n/4.0) * X_leng));
    setFixedSize(X_leng,Y_leng);

    for(size_t i = 0; i<but_n; i++)
    {
        Buttons[i].resize(X_leng/2,X_leng/2);
        if(i)
        {
            QPixmap map(X_leng/2,X_leng/2);
            map.fill(Qt::GlobalColor(i+1));

            QIcon icon;
            icon.addPixmap(map,QIcon::Normal,QIcon::On);

            Buttons[i].setIcon(icon);
            Buttons[i].setIconSize(map.rect().size().boundedTo(QSize(X_leng/3,X_leng/3)));

            Buttons[i].resize(X_leng/2,X_leng/2);
            Buttons[i].move( (i%2)*(X_leng/2), ((i/2)%(i+1))*(X_leng/2) );
        }
        else
        {
            Buttons[i].setText(tr("PenSize"));
        }
    }

}

SettingsMenu::~SettingsMenu()
{
    delete[] Buttons;
}

void SettingsMenu::toggleWindow(const int x, const int y)
{
    if(isHidden())
    {
        move(x,y);
        show();
    }
    else
        hide();
}

void SettingsMenu::chooseColour(PushButton* pointer)
{
    if(pointer - Buttons)
        emit sendColour( (QColor(Qt::GlobalColor((pointer-Buttons)+1)).rgba()) );
    else
        PenSelection();
}

void SettingsMenu::PenSelection()
{

}
