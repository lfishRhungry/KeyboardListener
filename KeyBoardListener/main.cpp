#include "KeyBoardListener.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	KeyBoardListener w;
	w.show();
	return a.exec();
}
