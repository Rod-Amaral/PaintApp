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
    : QWidget(parent, Qt::Window), Buttons(new PushButton[but_n]),
      penSizeBox(this), penSetSizeButton(new PushButton[penBut_n]), setColorScreen(this),
      PenSettings_buttons(new PushButton[4]), BrushStyle_buttons(new PushButton[17]),
      PenStyle_buttons(new PushButton[6]), PenCap_buttons(new PushButton[3]), PenJoin_buttons(new PushButton[4])
{
    setFixedSize(X_leng,Y_leng);

    //Special Buttons
    for(size_t i = 0; i<special_b; i++)
    {
        Buttons[i].setParent(this);
        Buttons[i].resize(X_leng/2,X_leng/2);
        Buttons[i].move( (i%2)*(X_leng/2), ((i/2)%(i+1))*(X_leng/2) );
    }
    Buttons[0].setText(tr("Pen\nSize"));
    Buttons[1].setText(tr("Colour"));
    Buttons[2].setText(tr("Custom\nPen"));
    Buttons[3].setText(tr("Sync"));

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

    //Predifined Buttons Icons for PenSize
    for(size_t i = 0; i<(penBut_n-1); i++)
    {
        static QPainter painter;

        //Making Icons of a point with the different PenSizes, to be used by the buttons
        QPixmap map(X_leng/2,X_leng/2);
        map.fill(Qt::white);
        painter.begin(&map);
        painter.setPen(QPen(QBrush(Qt::black),(i+1)*4,Qt::SolidLine,Qt::RoundCap));
        painter.drawPoint(map.width()/2,map.height()/2);

        QIcon icon;
        icon.addPixmap(map,QIcon::Normal,QIcon::On);

        penSetSizeButton[i].setIcon(icon);
        penSetSizeButton[i].setIconSize(map.rect().size().boundedTo(QSize(X_leng/2-1,X_leng/2-1)));

        painter.end();
    }

    //Pen Settings (PenSettings_buttons)
    setPenButtons(PenSettings_buttons,4,0);
    setPenSettings_names(PenSettings_buttons);

    //BrushStyle Buttons (BrushStyle_buttons)
    setPenButtons(BrushStyle_buttons, 17, (X_leng/2));
    setBrushStyle_names(BrushStyle_buttons);

    //PenStyle Buttons (PenStyle_buttons)
    setPenButtons(PenStyle_buttons, 6, 0);
    setPenStyle_names(PenStyle_buttons);

    //PenCapStyle Buttons (PenCap_buttons)
    setPenButtons(PenCap_buttons, 3, 0);
    setPenCapStyle_names(PenCap_buttons);

    //PenJoinStyle Buttons (PenJoin_buttons)
    setPenButtons(PenJoin_buttons, 4, 0);
    setPenJoinStyle_names(PenJoin_buttons);

}

void SettingsMenu::setPenButtons(PushButton* const & buttons, uint8_t amount, uint16_t extraSize)
{
    for(size_t i = 0; i<amount; i++)
    {
        //Parent
        buttons[i].setParent(this);

        //Resize and move
        buttons[i].resize(X_leng+extraSize,boxHeight);
        buttons[i].move(0,i*boxHeight);
    }
}

void SettingsMenu::set()
{
    //Open and Close Settings Menu Connections
    QObject::connect((MainWindow*)parentWidget(), &MainWindow::toggleSettingsWindow,
                     this, &SettingsMenu::toggleWindow);
    QObject::connect((MainWindow*)parentWidget(), &MainWindow::closeSettingsWindow,
                     this, &SettingsMenu::close);

    //Connections for Setting values of the MainWindow Pen
    QObject::connect(this, &SettingsMenu::sendColour,
                     (MainWindow*)parentWidget(), &MainWindow::setColour );
    QObject::connect(this, &SettingsMenu::sendPenSize,
                     (MainWindow*)parentWidget(), &MainWindow::setPenSize);
    QObject::connect(this, &SettingsMenu::sendBrushStyle,
                     (MainWindow*)parentWidget(), &MainWindow::setBrushStyle);
    QObject::connect(this, &SettingsMenu::sendPenStyle,
                     (MainWindow*)parentWidget(), &MainWindow::setPenStyle);
    QObject::connect(this, &SettingsMenu::sendPenCapStyle,
                     (MainWindow*)parentWidget(), &MainWindow::setPenCapStyle);
    QObject::connect(this, &SettingsMenu::sendPenJoinStyle,
                     (MainWindow*)parentWidget(), &MainWindow::setPenJoinStyle);

    //Connections for Special buttons functions
    for(size_t i = 0; i<special_b; i++)
    {
        QObject::connect(Buttons+i, &PushButton::sendPointer,
                         this, &SettingsMenu::chooseSettings);
    }

    //Colour Buttons Connections
    for(size_t i = 4; i<but_n; i++)
    {
        Buttons[i].setParent(this);
        QObject::connect(Buttons+i, &PushButton::sendPointer,
                         this, &SettingsMenu::chooseColour);
    }

    //Set Custom PenSize button, in PenSize menu
    QObject::connect(penSetSizeButton+(penBut_n-1), &PushButton::clicked,
                    this, &SettingsMenu::checkPenSize);

    //Connections for pre-defined PenSize Buttons
    for(size_t i = 0; i<(penBut_n-1) ;i++)
    {
        QObject::connect(penSetSizeButton+i, &PushButton::sendPointer,
                         this, &SettingsMenu::choosePenSize);
    }

    //Ok, Collor window
    QObject::connect(&setColorScreen, &QColorDialog::colorSelected,
                     this, &SettingsMenu::chooseQColour);

    //Connection of Pen Settings button, to correpsonding following menus
    QObject::connect(PenSettings_buttons, &PushButton::clicked,
                     this, &SettingsMenu::askBrushStyle);
    QObject::connect(PenSettings_buttons+1, &PushButton::clicked,
                     this, &SettingsMenu::askPenStyle);
    QObject::connect(PenSettings_buttons+2, &PushButton::clicked,
                     this, &SettingsMenu::askPenCapStyle);
    QObject::connect(PenSettings_buttons+3, &PushButton::clicked,
                     this, &SettingsMenu::askPenJoinStyle);

    //Connections between BrushStyle buttons and data sending function
    for(size_t i = 0; i<17; i++)
    {
        QObject::connect(BrushStyle_buttons+i, &PushButton::sendPointer,
                         this, &SettingsMenu::chooseBrushStyle);
    }

    //Connections between PenStyle buttons and data sending function
    for(size_t i = 0; i<6; i++)
    {
        QObject::connect(PenStyle_buttons+i, &PushButton::sendPointer,
                         this, &SettingsMenu::choosePenStyle);
    }

    //Connections between PenCapStyle buttons and data sending function
    for(size_t i = 0; i<3; i++)
    {
        QObject::connect(PenCap_buttons+i, &PushButton::sendPointer,
                         this, &SettingsMenu::choosePenCapStyle);
    }

    //Connections between PenJoinStyle buttons and data sending function
    for(size_t i = 0; i<4; i++)
    {
        QObject::connect(PenJoin_buttons+i, &PushButton::sendPointer,
                         this, &SettingsMenu::choosePenJoinStyle);
    }
}

SettingsMenu::~SettingsMenu()
{
    delete[] Buttons;
    delete[] penSetSizeButton;
    delete[] PenSettings_buttons;
    delete[] BrushStyle_buttons;
    delete[] PenStyle_buttons;
    delete[] PenCap_buttons;
    delete[] PenJoin_buttons;
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

//Changes to the ask Custom Colour menu
void SettingsMenu::askCustomColour()
{
    //Hide Settings Menu
    hide();

    setColorScreen.show();
}

//Changes to the ask which Pen Setting you want to change
void SettingsMenu::askPenSettings()
{
    //Hide buttons
    for(size_t i = 0; i<but_n; i++)
        Buttons[i].hide();

    setFixedSize(X_leng, boxHeight*4);
    for(size_t i = 0; i<4; i++)
        PenSettings_buttons[i].show();
}

//Checks PenSize in the box and sends that value to MainWindow
void SettingsMenu::checkPenSize()
{
    emit sendPenSize(penSizeBox.value());
    close();
}


//the ask functions show the butons for the selected menu
void SettingsMenu::askBrushStyle()
{
    for(size_t i = 0; i<4; i++)
        PenSettings_buttons[i].hide();

    setFixedSize(X_leng+(X_leng/2), boxHeight*17);
    for(size_t i = 0; i<17; i++)
        BrushStyle_buttons[i].show();
}

void SettingsMenu::askPenStyle()
{
    for(size_t i = 0; i<4; i++)
        PenSettings_buttons[i].hide();

    for(size_t i = 0; i<6; i++)
        PenStyle_buttons[i].show();
}

void SettingsMenu::askPenCapStyle()
{
    for(size_t i = 0; i<4; i++)
        PenSettings_buttons[i].hide();

    setFixedSize(X_leng, boxHeight*3);
    for(size_t i = 0; i<3; i++)
        PenCap_buttons[i].show();
}

void SettingsMenu::askPenJoinStyle()
{
    for(size_t i = 0; i<4; i++)
        PenSettings_buttons[i].hide();

    setFixedSize(X_leng, boxHeight*4);
    for(size_t i = 0; i<4; i++)
        PenJoin_buttons[i].show();
}

void SettingsMenu::askSync()
{
    //Sorry again
    //Here I ask to clear the Child Window, and start Syncing
    ((MainWindow*)parent())->BCP_SendThread->setOP_code(2);
    ((MainWindow*)parent())->BCP_SendThread->start();
    ((MainWindow*)parent())->stop = false;
    ((MainWindow*)parent())->SyncImages();
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

    for(size_t i = 0; i<but_n; i++)
        Buttons[i].show();

    penSizeBox.hide();
    for(size_t i = 0; i<penBut_n; i++)
        penSetSizeButton[i].hide();
    for(size_t i = 0; i<4; i++)
        PenSettings_buttons[i].hide();
    for(size_t i = 0; i<17; i++)
        BrushStyle_buttons[i].hide();
    for(size_t i = 0; i<6; i++)
        PenStyle_buttons[i].hide();
    for(size_t i = 0; i<3; i++)
        PenCap_buttons[i].hide();
    for(size_t i = 0; i<4; i++)
        PenJoin_buttons[i].hide();
}

//Using the Enter key to set custom PenSize
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

void SettingsMenu::chooseQColour(QColor colour)
{
    emit sendColour(colour.rgba());
}

void SettingsMenu::choosePenSize(PushButton* pointer)
{
    emit sendPenSize((pointer-penSetSizeButton+1)*4);
    close();
}

void SettingsMenu::chooseBrushStyle(PushButton* pointer)
{
    emit sendBrushStyle(Qt::BrushStyle(pointer-BrushStyle_buttons+1));
    close();
}

void SettingsMenu::choosePenStyle(PushButton* pointer)
{
    emit sendPenStyle(Qt::PenStyle(pointer-PenStyle_buttons));
    close();
}

void SettingsMenu::choosePenCapStyle(PushButton* pointer)
{
    emit sendPenCapStyle(Qt::PenCapStyle((pointer-PenCap_buttons)*0x10));
    close();
}

void SettingsMenu::choosePenJoinStyle(PushButton* pointer)
{
    emit sendPenJoinStyle(Qt::PenJoinStyle(
                              ((pointer-PenJoin_buttons)<3) ? ((pointer-PenJoin_buttons)*0x40) : 0x100 ));
    close();
}

//for the 4 special buttons
void SettingsMenu::chooseSettings(PushButton* pointer)
{
    switch(pointer-Buttons)
    {
        case 0:
            askPenSize();
            break;
        case 1:
            askCustomColour();
            break;
        case 2:
            askPenSettings();
            break;
        case 3:
            askSync();
            break;
    }
}

void SettingsMenu::setPenSettings_names(PushButton*const & buttons)
{
    buttons[0].setText(tr("Brush Style"));
    buttons[1].setText(tr("Pen Style"));
    buttons[2].setText(tr("PenCap Style"));
    buttons[3].setText(tr("PenJoin Style"));
}

void SettingsMenu::setBrushStyle_names(PushButton* const & buttons)
{
    buttons[0].setText(tr("NoBrush"));
    buttons[1].setText(tr("Dense1Pattern"));
    buttons[2].setText(tr("Dense2Pattern"));
    buttons[3].setText(tr("Dense3Pattern"));
    buttons[4].setText(tr("Dense4Pattern"));
    buttons[5].setText(tr("Dense5Pattern"));
    buttons[6].setText(tr("Dense6Pattern"));
    buttons[7].setText(tr("Dense7Pattern"));
    buttons[8].setText(tr("HorPattern"));
    buttons[9].setText(tr("VerPattern"));
    buttons[10].setText(tr("CrossPattern"));
    buttons[11].setText(tr("BDiagPattern"));
    buttons[12].setText(tr("FDiagPattern"));
    buttons[13].setText(tr("DiagCrossPattern"));
    buttons[14].setText(tr("LinearGradientPattern"));
    buttons[15].setText(tr("ConicalGradientPattern"));
    buttons[16].setText(tr("RadialGradientPattern"));
}

void SettingsMenu::setPenStyle_names(PushButton* const & buttons)
{
    buttons[0].setText(tr("NoPen"));
    buttons[1].setText(tr("SolidLine"));
    buttons[2].setText(tr("DashLine"));
    buttons[3].setText(tr("DotLine"));
    buttons[4].setText(tr("DashDotLine"));
    buttons[5].setText(tr("DashDotDotLine"));
}

void SettingsMenu::setPenCapStyle_names(PushButton* const & buttons)
{
    buttons[0].setText(tr("FlaCap"));
    buttons[1].setText(tr("SquareCap"));
    buttons[2].setText(tr("RoundCap"));
}

void SettingsMenu::setPenJoinStyle_names(PushButton* const & buttons)
{
    buttons[0].setText(tr("MiterJoin"));
    buttons[1].setText(tr("BevelJoin"));
    buttons[2].setText(tr("RoundJoin"));
    buttons[3].setText(tr("SVGMiterJoin"));
}
