#include <QApplication>
#include "MainWindow.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

    MainWindow window1;
    ChildWindow window2(&window1);

    window1.setWindowTitle("Main Window");
    window2.setWindowTitle("Child Window");

    //Point painting
    QObject::connect(&window1,&MainWindow::doPaintPoint,
                    &window2,&ChildWindow::PaintPoint);

    //Paint Line
    QObject::connect(&window1,&MainWindow::doPaintLine,
                    &window2,&ChildWindow::PaintLine);

   //Clear Image
   QObject::connect(&window1,&MainWindow::doclearImage,
                    &window2,&ChildWindow::ClearImage);

    window2.show();
	window1.show();
	return app.exec();
}
