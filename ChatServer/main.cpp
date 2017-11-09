#include <QCoreApplication>
#include <QThread>

#include "server.h"
#include "HttpService.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	Server::InitServer(a);

    return a.exec();
}
