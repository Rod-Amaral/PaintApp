#include <QApplication>
#include "ChildWindow.h"
#include "SettingsMenu.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

    MainWindow window1;
    ChildWindow window2;
    SettingsMenu Settings(&window1);

    //Boolean Communication Protocol - BCP
    QObject::connect(&window1, &MainWindow::SEND_BIT,
                     &window2, &ChildWindow::IN_BIT);

    QObject::connect(&window2, &ChildWindow::PARITY_SEND,
                     &window1, &MainWindow::PARITY_IN);

    window1.setWindowTitle("Main Window");
    window2.setWindowTitle("Child Window");
    Settings.setWindowTitle("Pick");

    //Some stuff to do
    window1.set();
    Settings.set();

    window2.move(0,0);
    window1.move(1920,0);
    window2.show();
	window1.show();

	return app.exec();
}
