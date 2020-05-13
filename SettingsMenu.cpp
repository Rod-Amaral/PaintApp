#include "SettingsMenu.h"
#include <QDebug>

//Number of buttons and non-color buttons
static const uint8_t but_n(21);
static const uint8_t special_b(4);

//Settings menu size constans
static const uint16_t X_leng(120);
static const uint16_t Y_leng(((but_n/4.0 + 0.25) * X_leng));

//Pen Settings button values
static const uint8_t penBut_n(5);
static const uint8_t boxHeight(30);

//Redirecting the clicked signal to another functions
PushButton::PushButton(QWidget* const parent) : QPushButton(parent)
{
    QObject::connect(this, &PushButton::clicked,
                     this, &PushButton::wasClicked);
}

//This function emits a signal when the button is clicked, with the pointer of the button as data
void PushButton::wasClicked()
{
    emit sendPointer(this);
}

SettingsMenu::SettingsMenu(QWidget* const parent)
    : QWidget(parent, Qt::Window), Buttons(new PushButton[but_n]), penSizeBox(this)
{
    setFixedSize(X_leng,Y_leng);

    //Special Buttons
    for(size_t i = 0; i<special_b; i++)
    {
        Buttons[i].setParent(this);
        Buttons[i].resize(X_leng/2,X_leng/2);
        Buttons[i].move( (i%2)*(X_leng/2), ((i/2)%(i+1))*(X_leng/2) );
    }
    Buttons[0].setText(tr("PenSize"));
    Buttons[1].setText(tr("Colour"));

    //Set PenSize button
    QObject::connect(Buttons, &PushButton::sendPointer,
                     this, &SettingsMenu::chooseSettings);

    //Colour Buttons connections
    for(size_t i = 4; i<but_n; i++)
    {
        Buttons[i].setParent(this);
        QObject::connect(Buttons+i, &PushButton::sendPointer,
                         this, &SettingsMenu::chooseColour);
    }

    //Colour Buttons
    for(size_t i = special_b; i<but_n; i++)
    {
        QPixmap map(X_leng/2,X_leng/2);
        map.fill(Qt::GlobalColor(i-special_b+2));

        QIcon icon;
        icon.addPixmap(map,QIcon::Normal,QIcon::On);

        Buttons[i].setIcon(icon);
        Buttons[i].setIconSize(map.rect().size().boundedTo(QSize(X_leng/3,X_leng/3)));

        Buttons[i].resize(X_leng/2,X_leng/2);
        Buttons[i].move( (i%2)*(X_leng/2), ((i/2)%(i+1))*(X_leng/2) );
    }

    //PenSize Buttons
    penSetSizeButton = new PushButton[penBut_n];
    for(size_t i = 0; i<penBut_n; i++)
        penSetSizeButton[i].setParent(this);

    //Pre-defined PenSize buttons
    for(size_t i = 0; i<(penBut_n-1); i++)
    {
        penSetSizeButton[i].resize(X_leng/2,X_leng/2);
        penSetSizeButton[i].move( ((i)%2)*(X_leng/2), (((i)/2)%(i+1))*(X_leng/2));
    }

    //Setting box for PenSize value
    penSizeBox.setRange(1,100);
    penSizeBox.setSingleStep(1);
    penSizeBox.setValue(0);
    penSizeBox.setPrefix(tr("PenSize: "));
    penSizeBox.setGeometry(0,X_leng,X_leng,boxHeight);

    //Button below PenSize value box
    penSetSizeButton[penBut_n-1].setGeometry(0,boxHeight+X_leng,X_leng,boxHeight);
    penSetSizeButton[penBut_n-1].setText(tr("Set PenValue"));

    //Set custom PenSize button
    QObject::connect(penSetSizeButton+(penBut_n-1), &PushButton::clicked,
                    this, &SettingsMenu::checkPenSize);

    //Predifined Buttons for PenSize
    for(size_t i = 0; i<(penBut_n-1); i++)
    {
        QObject::connect(penSetSizeButton+i, &PushButton::sendPointer,
                         this, &SettingsMenu::choosePenSize);
    }
}

void SettingsMenu::set()
{

}

SettingsMenu::~SettingsMenu()
{
    delete[] Buttons;
    delete[] penSetSizeButton;
}

//Changes to the ask PenSize menu
void SettingsMenu::askPenSize()
{
    //Hide buttons
    for(size_t i = 0; i<but_n; i++)
        Buttons[i].hide();

    //Sorry about this one
    penSizeBox.setValue( ((MainWindow*)(this->parentWidget()))->pen.width());
    setFixedSize(X_leng,2*boxHeight+((penBut_n-1)/2)*(X_leng/2));

    //Show Buttons
    penSizeBox.show();
    for(size_t i = 0; i<penBut_n; i++)
        penSetSizeButton[i].show();
}

//Checks PenSize in the box and sends that value to MainWindow
void SettingsMenu::checkPenSize()
{
    emit sendPenSize(penSizeBox.value());
    close();
}

//Toggles settings menu, and when openeing moves it to the received mouse coordinate from main window
void SettingsMenu::toggleWindow(const int x, const int y)
{
    if(isHidden())
    {
        showNormal();
        move(x,y);
        show();
    }
    else
        hide();
}

//Resets window to default menu
void SettingsMenu::showEvent(QShowEvent *event)
{
    setFixedSize(X_leng,Y_leng);

    penSizeBox.hide();
    for(size_t i = 0; i<penBut_n; i++)
        penSetSizeButton[i].hide();
    for(size_t i = 0; i<but_n; i++)
        Buttons[i].show();
}

void SettingsMenu::keyPressEvent(QKeyEvent *event)
{
    //If selecting PenSize, pressing enter will set PenSize
    if( (event->key() == 16777220) && penSetSizeButton[0].isVisible())
    {
        checkPenSize();
        close();
    }
}

void SettingsMenu::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
        close();
}

//Determines which button was pressed by its location in the button array, and hence determining which colour to set
void SettingsMenu::chooseColour(PushButton* pointer)
{
    emit sendColour( (QColor(Qt::GlobalColor(pointer-Buttons-special_b+2)).rgba()) );
    close();
}

void SettingsMenu::choosePenSize(PushButton* pointer)
{
    emit sendPenSize((pointer-penSetSizeButton+1)*4);
    close();
}

//for the 4 scpecial buttons
void SettingsMenu::chooseSettings(PushButton* pointer)
{
    switch(pointer-Buttons)
    {
        case 0:
            askPenSize();
            break;
    }
}
