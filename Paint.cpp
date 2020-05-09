#include <QApplication>
#include "MainWindow.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

    MainWindow window1;
    ChildWindow window2;

    window1.setWindowTitle("Main Window");
    window2.setWindowTitle("Child Window");

    //Boolean Communication Protocol - BCP
    QObject::connect(&window1, &MainWindow::SEND_BIT,
                     &window2, &ChildWindow::IN_BIT);
    //Simplex for now

    window2.show();
	window1.show();
	return app.exec();
}
