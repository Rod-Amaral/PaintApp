#include <QApplication>
#include "ChildWindow.h"
#include "SettingsMenu.h"

bool BIT(0);
bool DO_READ(0);

bool RESEND(0);

int main(int argc, char** argv)
{
    QApplication app(argc, argv);


    MainWindow window1;
    ChildWindow window2;
    window2.BCP_ReceiveThread->start();

    SettingsMenu Settings(&window1);

    /*
    QObject::connect(&window2, &ChildWindow::SendPixel,
                     &window1, &MainWindow::SyncImages);*/

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
