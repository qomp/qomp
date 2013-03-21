#include <QApplication>
#include "qomp.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setApplicationName("Qomp");
	a.setApplicationVersion("0.1 beta");
	Qomp qomp;
	qomp.init();
	return a.exec();
}
