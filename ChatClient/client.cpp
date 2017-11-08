#include "client.h"
#include "mainwindow.h"

#include <iostream>

#include <QWidget>

// Static members
QUdpSocket* Client::m_socket;
MainWindow* Client::window;
QHostAddress Client::currentHostAddress;

/* Fills our std::map with all the command functions*/
StringToFunctioMap Client::commandMap =
{
	{ commandPair(CONNECTTOSERVER_KEY, Client::Connect) },
	{ commandPair(DISCONNECTFROMSERVER_KEY, Client::Disconnect) }
};

Client::Client()
{
}

Client::~Client()
{
}

void Client::run()
{
	currentHostAddress = QHostAddress::Null;

	m_socket = new QUdpSocket();
	m_socket->bind();

	Client::Receive();
}


// Connects to the IP Address that was put in
void Client::Join(const QHostAddress& address)
{
    // Sends a command string to the server    
    Send(m_socket, CommandInfo(CONNECTTOSERVER_KEY, CONNECTTOSERVER_KEY.size(),address, SERVER_PORT));
}

// How the client wants to connect to the server
void Client::Connect(const CommandInfo & info)
{
    if (currentHostAddress != info.address)
    {
        currentHostAddress = info.address;
        //m_socket->connectToHost(info.address, info.port);
		
        window->PrintMessage("Connected to host at address: " + currentHostAddress.toString() + ":" + info.port);
        window->PrintMessage("Use the command '/name' to change your name");
			
        //window->setWindowTitle(QString::fromStdString(info.args));
    }
    else
    {
        window->PrintMessage("You're already connected to host at address: " + currentHostAddress.toString());
    }

    // Makes it possible to push the disconnect button
}

void Client::Disconnect(const CommandInfo & info)
{
	
	if (!currentHostAddress.isNull())
	{
		//Disconnection code
		Send(m_socket, CommandInfo(DISCONNECTFROMSERVER_KEY, DISCONNECTFROMSERVER_KEY.size(), currentHostAddress, SERVER_PORT));
		//m_socket->disconnect();
		window->ClearMessages();
		window->PrintMessage("Disconnected from " + currentHostAddress.toString());
		currentHostAddress = QHostAddress::Null;
	}
	else
	{
		window->PrintMessage("You have to be connected to a server to disconnect from it!");
	}
}

void Client::Send(const std::string& message)
{
	if (!currentHostAddress.isNull())
	{
		Send(m_socket, CommandInfo(message, message.size(), currentHostAddress, SERVER_PORT));
	}
}


void Client::StartThread(MainWindow * w)
{
	Client* client = new Client();
	client->window = w;

	Client::connect(client, SIGNAL(finished()), w, SLOT(quit()));
	client->start();
}


// Checks for and interprets command received in our QUdpSocket
void Client::Receive()
{
	while (true)
	{
		if (m_socket->waitForReadyRead())
		{
			CommandInfo info;
			char bfr[1024];
			m_socket->readDatagram(bfr, 1024, &info.address, &info.port);
			info.buffer = bfr;
			InterpretCommand(info);
		}
	}
}

// Wrapper for QUdpSocket method writeDatagram
void Client::Send(QUdpSocket* socket, const CommandInfo& info)
{
    socket->writeDatagram(&info.buffer[0], info.buffer.size() + 1, info.address, info.port);
}

// Interprets the data we got in the receive method
void Client::InterpretCommand(const CommandInfo& info)
{

    if (info.size != 0)
    {
        // Checks if the text was a command or not
        if (info.buffer[0] == '/')
        {
            // Finds the command string
            size_t spacePosition = info.buffer.find(' ');
            std::string command = info.buffer.substr(0, spacePosition);

            
            // Checks if the command string was a valid command function
            StringToFunctioMap::iterator itr = commandMap.find(command);
            if (itr == commandMap.end())
            {
                // Didn't find the command, handle it!
				window->PrintMessage(QString::fromStdString("\'" + command + "\'" + " is not a valid command!" + "Write /help for a list of all commands!"));
            }
            else
            {
              
                CommandInfo newInfo = info;
                if (spacePosition < 100)
                {
                    newInfo.args = info.buffer.substr(spacePosition + 1, info.buffer.size() - 1);
                }
                // Calls the command function
                itr->second(newInfo);
            }
        }
        else
        {
			window->PrintMessage(QString::fromStdString(info.buffer));
        }
    }
}