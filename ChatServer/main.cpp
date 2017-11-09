#include <QCoreApplication>
#include <QThread>

#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	Server::InitServer();

    return a.exec();
}
