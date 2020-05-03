#include <QApplication>
#include "WindowClass.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	MahWindow window1;
	childWindow window2(&window1);

	window1.setWindowTitle("Main Window");
	window2.setWindowTitle("Child Window");

	QObject::connect(&window1,&MahWindow::pointChanged,
					&window2,&childWindow::setQPoint);

	window2.show();
	window1.show();
	return app.exec();
}