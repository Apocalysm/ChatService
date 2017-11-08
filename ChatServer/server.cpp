#include "Server.h"
#include <iostream>
#include <future>
#include <thread>
#include <experimental/filesystem>

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>
#include <QString>

#include "client.h"

// Static members
QUdpSocket* Server::m_socket = new QUdpSocket();
std::vector<Client*> Server::clients = std::vector<Client*>();
StringToFunctioMap Server::commandMap = StringToFunctioMap();

std::string Server::name = "Default";

Server::Server()
{
    // Binds our static socket to the specied SERVER_PORT
    m_socket->bind(SERVER_PORT);
    //m_socket->setBlocking(false);

    /* Inserts all the command-functions into an unordered_map */
    commandMap.insert(commandPair(CONNECTTOSERVER_KEY, Server::Connect));
    commandMap.insert(commandPair(DISCONNECTFROMSERVER_KEY, Server::Disconnect));
	commandMap.insert(commandPair(CHANGENAME_KEY, Server::ChangeName));
	commandMap.insert(commandPair(WHISPER_KEY, Server::Whisper));
}


Server::~Server()
{
}


void Server::Update()
{

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
		CommandInfo newInfo(CONNECTTOSERVER_KEY, sizeof(CONNECTTOSERVER_KEY), info.address, info.port);
		Send(m_socket, newInfo);

		std::cout << "Client at: " << QString(info.address.toString()).toStdString() << ":" << info.port << " connected" << std::endl;
		// Adds the client to our vector
		clients.push_back(new Client(QHostAddress(info.address), info.port, ""));
	}
}

void Server::Disconnect(const CommandInfo & info)
{
	Client* c = GetClient(info);

	auto itr = std::find(clients.begin(), clients.end(), c);
    clients.erase(itr);

    std::cout << c->GetName() << " has disconnected" << std::endl;

    delete c;
}

void Server::ChangeName(const CommandInfo & info)
{
	// Gets the client communicating with the server
	Client* c = GetClient(info);
	
	CommandInfo newInfo = info;

	if (c->GetName() != info.args)
	{
		bool canChange = true;
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
	if (info.buffer.size() >= WHISPER_KEY.size() + 4 && info.args[0] != ' ')
	{
		// Gets the name the client is whispering to 
		size_t spacePosition = info.args.find(' ');
		std::string name = info.args.substr(0, spacePosition);

		// Gets the whisper messsage from the args
		std::string message = info.args.substr(spacePosition + 1, info.buffer.size() - 1);

		if (message.size() > 0)
		{
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
	}
}

// Wrapper for sf::UdpSocket method 'send'
void Server::Send(QUdpSocket* socket, const CommandInfo & info)
{
    socket->writeDatagram(&info.buffer[0], info.buffer.size() + 1, info.address, info.port);
}

// Wrapper for sf::UdpSocket method 'receive'
void Server::Receive()
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
    if (info.size != 0)
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