#include <QCoreApplication>
#include <QThread>

#include "server.h"

class ReceiveThread : public QThread
{
private:
	void run()
	{
		Server::Receive();
	}
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	Server server;
	Server::Receive();
	/*ReceiveThread thread;
	ReceiveThread::connect(&thread, SIGNAL(finished()), &a, SLOT(quit()));
	thread.start();*/

    return a.exec();
}
