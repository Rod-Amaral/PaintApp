#include <QApplication>
#include <QTextStream>
#include "ChildWindow.h"
#include "MainWindow.h"

static QTextStream out(stderr); //Debug

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

    MainWindow window1;
    ChildWindow window2(&window1);

    window1.setWindowTitle("Main Window");
    window2.setWindowTitle("Child Window");

    QObject::connect(&window1,&MainWindow::pointChanged,
                    &window2,&ChildWindow::PaintPoint);

    //window2.show();
	window1.show();
	return app.exec();
}
