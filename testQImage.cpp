#include <QApplication>
#include <QBrush>
#include <QPen>
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>
#include <QWidget>
#include <QImage>

static const uint16_t X_leng = 700;
static const uint16_t Y_leng = 800;

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QWidget window1;
	window1.setFixedSize(X_leng,Y_leng);

	window1.setWindowTitle("Main Window");

	QImage image(20,20,QImage::Format_Mono);

	QPainter painter(&window1);
	painter.drawImage(QRect(40,40,20,20),image);
	painter.end();

	window1.show();
	return app.exec();
}