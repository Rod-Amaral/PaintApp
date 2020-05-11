#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include <QWidget>

class MainWindow;
class SettingsMenu : public QWidget
{
    Q_OBJECT
public:
    SettingsMenu(QWidget* const parent = 0);

private:

public slots:
    void toggleWindow(const int x, const int y);

};

#endif // SETTINGSMENU_H
