#pragma once

#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QThread>

#include "common.h"

class MainWindow;

// Chat client which inherits from QThread 
// so all of the member functions can run in a thread separate from the main one
class Client : public QThread
{
public:
    Client();
    ~Client();

	void run();

    static void Receive();

    /* Command functions */
    static void Join(const QHostAddress&);
    static void Connect(const CommandInfo& info);
    static void Disconnect(const CommandInfo& info);

    static void Send(const std::string& message);
	static void StartThread(MainWindow* w);

	static void SetWindow(MainWindow* w);

private:
	static MainWindow* window;

    static QUdpSocket* m_socket;

    static QHostAddress currentHostAddress;

    static StringToFunctioMap commandMap;

    static void InterpretCommand(const CommandInfo& info);

    static void Send(QUdpSocket* socket, const CommandInfo& info);
};
