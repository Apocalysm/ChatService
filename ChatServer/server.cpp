#include "Server.h"
#include <iostream>
#include <future>
#include <thread>
#include <experimental/filesystem>

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QString>
#include <QEventLoop>

#include "client.h"
#include "HttpService.h"

// Static members
QUdpSocket* Server::m_socket;
std::vector<Client*> Server::clients = std::vector<Client*>();
StringToFunctioMap Server::commandMap = StringToFunctioMap();
std::unordered_map<std::string, std::string> Server::functionHelpMap = std::unordered_map<std::string, std::string>();
HttpService* Server::service = nullptr;

std::string Server::name = "Default";

Server::Server()
{
    // Binds our static socket to the specied SERVER_PORT

    //m_socket->setBlocking(false);

    /* Inserts all the command-functions into an unordered_map */
    commandMap.insert(commandPair(CMD_CONNECTTOSERVER.Key(), Server::Connect));
    commandMap.insert(commandPair(CMD_DISCONNECTFROMSERVER.Key(), Server::Disconnect));
	commandMap.insert(commandPair(CMD_CHANGENAME.Key(), Server::ChangeName));
	commandMap.insert(commandPair(CMD_WHISPER.Key(), Server::Whisper));
	commandMap.insert(commandPair(CMD_BTC.Key(), Server::GetBitcoin));
	commandMap.insert(commandPair(CMD_HELP.Key(), Server::Help));

	/* Inserts all command help strings into an unordered map*/
	functionHelpMap.insert(stringPair(CMD_CONNECTTOSERVER.Key(), CMD_CONNECTTOSERVER.Help()));
	functionHelpMap.insert(stringPair(CMD_DISCONNECTFROMSERVER.Key(), CMD_DISCONNECTFROMSERVER.Help()));
	functionHelpMap.insert(stringPair(CMD_CHANGENAME.Key(), CMD_CHANGENAME.Help()));
	functionHelpMap.insert(stringPair(CMD_WHISPER.Key(), CMD_WHISPER.Help()));
	functionHelpMap.insert(stringPair(CMD_BTC.Key(), CMD_BTC.Help()));
	functionHelpMap.insert(stringPair(CMD_HELP.Key(), CMD_HELP.Help()));
}

Server::~Server()
{
	//Delete all clients from the server
	for (auto it = clients.begin(); it != clients.end();)
	{
		delete (*it);
		clients.erase(it);
	}

	delete m_socket;
}

// The connecting process of the client with login, register etc.
void Server::Connect(const CommandInfo& info)
{
	bool addClient = true;
	if (clients.size() > 0)
	{
		Client* c = GetClient(info);
		addClient = (c == nullptr);
	}

	if(addClient)
	{
		// Sends a command prompt to the client to make them connect to the server
		CommandInfo newInfo(CMD_CONNECTTOSERVER.Key(), CMD_CONNECTTOSERVER.Key().size(), info.address, info.port);
		Send(m_socket, newInfo);

		std::cout << "Client at: " << QString(info.address.toString()).toStdString() << ":" << info.port << " connected" << std::endl;
		// Adds the client to our vector
		clients.push_back(new Client(QHostAddress(info.address), info.port, ""));
	}
}

//Disconnect command from client
void Server::Disconnect(const CommandInfo & info)
{
	//Get which client
	Client* c = GetClient(info);

	//Remove the client from the client vector
	auto itr = std::find(clients.begin(), clients.end(), c);
    clients.erase(itr);

    std::cout << c->GetName() << " has disconnected" << std::endl;

	//Delete the client from the server
    delete c;

	//Send a text message to the client 
	CommandInfo newInfo = info;
	newInfo.buffer = CMD_DISCONNECTFROMSERVER.Key();
	Send(m_socket, newInfo);
}

//Change name command
void Server::ChangeName(const CommandInfo & info)
{	
	CommandInfo newInfo = info;

	//Check if the name have any space in it
	if (info.args.find(' ') != info.args.npos)
	{
		newInfo.buffer = "You can't use spaces in your name!";
		Send(m_socket, newInfo);
		return;
	}


	// Gets the client communicating with the server
	Client* c = GetClient(info);

	//Check if the client is already using that name
	if (c->GetName() != info.args)
	{
		bool canChange = true;
		//Check if any other clients is using that name
		for (auto client : clients)
		{
			if (client->GetName() == info.args)
			{
				canChange = false;

				newInfo.buffer = "The user at " + QString(client->GetAddress().toString()).toStdString() + ":" + std::to_string(client->GetPort()) + " is already using the name \'" + info.args + "\'";
				Send(m_socket, newInfo);

				break;
			}
		}
		//Change the name
		if (canChange)
		{
			newInfo.buffer = "You're name was changed from " + ((c->GetName() == "") ? "nothing" : c->GetName()) + " to " + info.args;
			c->SetName(info.args);
			Send(m_socket, newInfo);
		}
	}
	else
	{
		newInfo.buffer = "You are already using this name!";
		Send(m_socket, newInfo);
	}
}

// /whisper
void Server::Whisper(const CommandInfo & info)
{	
	// Checks if the command was long enough to hold a /whisper command call
	if (info.buffer.size() < CMD_WHISPER.Key().size() + 4 || info.args[0] != ' ')
	{
		CommandInfo newInfo = info;
		newInfo.buffer = "help: " + CMD_WHISPER.Help();
		Send(m_socket, newInfo);
		return;
	}
	// Gets the name the client is whispering to 
	size_t spacePosition = info.args.find(' ');
	std::string name = info.args.substr(0, spacePosition);

	// Gets the whisper messsage from the args
	std::string message = info.args.substr(spacePosition + 1, info.buffer.size() - 1);

	if (message.size() <= 0)
	{
		CommandInfo newInfo = info;
		newInfo.buffer = "help: " + CMD_WHISPER.Help();
		Send(m_socket, newInfo);
		return;
	}

	// Gets the client receiving the whisper
	Client* receiver = GetClient(name);

	// Gets the client trying to whisper 
	Client* sender = GetClient(info);

	std::string receivedMessage = "<i>" + sender->GetName() + " whispers" + "</i>" + ": " + message;
	std::string sentMessage = "<i>To " + name + "</i>" + ": " + message;

	CommandInfo senderInfo = info;
	senderInfo.buffer = sentMessage;
	Send(m_socket, senderInfo);

	Send(m_socket, CommandInfo(receivedMessage, receivedMessage.size(), receiver->GetAddress(), receiver->GetPort()));

}

void Server::GetBitcoin(const CommandInfo & info)
{
	// QNetworkAccessManager* manager = new QNetworkAccessManager();
	// QNetworkReply* reply = manager->get(QNetworkRequest(QUrl("http://preev.com")));
	// reply->connect(manager, SIGNAL(finished(QNetworkReply*)), reply, SLOT(replyFinished(QNetworkReply*)));
	
	HttpService* service = new HttpService();
	service->MakeRequest();

	/*
	reply->waitForReadyRead(500);
	QByteArray data = reply->readAll();
	for (size_t i = 0; i < data.size(); i++)
	{
		std::cout << data[i] << std::endl;
	}
	*/
}

//Help command for client
void Server::Help(const CommandInfo & info)
{
	CommandInfo newInfo = info;
	newInfo.buffer = "help:<br>";
	newInfo.buffer += "/disconnect Disconnects from current host <br/>";
	newInfo.buffer += CMD_WHISPER.Help() + "<br/>";
	newInfo.buffer += CMD_CHANGENAME.Help() + "<br/>";
	Send(m_socket, newInfo);
}

// Wrapper for QUdpSocket method 'writeDatagram'
void Server::Send(QUdpSocket* socket, const CommandInfo & info)
{
    socket->writeDatagram(&info.buffer[0], info.buffer.size() + 1, info.address, info.port);
}

// SLOT: Wrapper for sf::UdpSocket method 'receive'
void Server::Receive()
{
    CommandInfo info;
	char bfr[1024];
	m_socket->readDatagram(bfr, 1024, &info.address, &info.port);
	info.buffer = bfr;
	InterpretCommand(info);
}

// Finds the client with the same ip and port as the one trying to communicate with the server
Client* Server::GetClient(const CommandInfo & info)
{
	for (auto client : clients)
	{
		if (client->GetAddress() == info.address &&
			client->GetPort() == info.port)
			return client;
	}
	std::cout << "New user (" << QString(info.address.toString()).toStdString() << ":" << info.port << ") trying to access the server!" << std::endl;
	return nullptr;
}

// Searches through all the clients trying to find one with the specified name
Client * Server::GetClient(const std::string & name)
{
	for (auto client : clients)
	{
		if (client->GetName() == name)
			return client;
	}
	return nullptr;
}

// Interprets the data we got in the receive method
void Server::InterpretCommand(const CommandInfo& info)
{
    if (info.buffer.size() != 0)
    {
		// Copies info
		CommandInfo newInfo = info;

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
                // Didn't find the command, handle it! /* REPLACE WITH SEND TO A ERROR FUNCTION IN THE CLIENTSC CLASS */
                std::cerr << '\'' << command << '\'' << " is not a valid command!" << std::endl << "Write /help for a list of all commands!" << std::endl;
            }
            else
            {
				// Checks if any arguments were passed in
				if (spacePosition != command.npos)
				{
					newInfo.args = info.buffer.substr(spacePosition + 1, info.buffer.size() - 1);
				}
				
				if (functionHelpMap[command].size() > 0)
				{
					if (newInfo.args.size() <= 0)
					{
						newInfo.buffer = "help: " + functionHelpMap[command];
						Send(m_socket, newInfo);
						return;
					}
				}
				// Calls the command function
				itr->second(newInfo);
            }
        }
        // Normal text message
        else
		{
			Client* c = GetClient(info);
			std::string name = ((c->GetName() == "") ? (QString(info.address.toString()).toStdString() + ":" + std::to_string(info.port)) : c->GetName());
			name += ": ";
			newInfo.buffer.insert(0, name);
            for (auto client : clients)
            {
				newInfo.address = client->GetAddress();
				newInfo.port = client->GetPort();
                Send(m_socket, newInfo);
            }
        }
    }
}


void Server::InitServer(const QCoreApplication& a)
{
	Server* server = new Server();
	//connect(server, SIGNAL(finished()), &a, SLOT(quit()));

	m_socket = new QUdpSocket();
	m_socket->bind(SERVER_PORT);
	connect(m_socket, &QUdpSocket::readyRead, server, &Server::Receive);

	//server->start();
}


void Server::run()
{
	QEventLoop loop;
	connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();
}
