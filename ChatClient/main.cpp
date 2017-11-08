#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	//MainWindow starts a thread in client
	MainWindow w;

	w.show();

    return a.exec();
}
