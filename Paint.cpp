#include <QApplication>
#include "MainWindow.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

    MainWindow window1;
    ChildWindow window2;

    window1.setWindowTitle("Main Window");
    window2.setWindowTitle("Child Window");

    //Paint Point
    QObject::connect(&window1,&MainWindow::doPaintPoint,
                    &window2,&ChildWindow::PaintPoint);

    //Paint Line
    QObject::connect(&window1,&MainWindow::doPaintLine,
                    &window2,&ChildWindow::PaintLine);

   //Clear Image
   QObject::connect(&window1,&MainWindow::doclearImage,
                    &window2,&ChildWindow::ClearImage);

   //Close Window
   QObject::connect(&window1,&MainWindow::doCloseWindow,
                    &window2,&ChildWindow::CloseWindow);

    window2.show();
	window1.show();
	return app.exec();
}
