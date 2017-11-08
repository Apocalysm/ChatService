#pragma once
#include <vector>

#include "common.h"

class Client;
class QUdpSocket;

// Server class is the subject in an observer pattern with the Client class as observers
class Server
{
public:
    Server();
    ~Server();

    void Update();
	static void Connect(const CommandInfo& info);
	static void Disconnect(const CommandInfo& info);
	static void ChangeName(const CommandInfo& info);
	static void Whisper(const CommandInfo& info);
	static void Help(const CommandInfo& info);

	static void Receive();
	static void Send(QUdpSocket* socket, const CommandInfo& info);

private:
    static QUdpSocket* m_socket;

    static std::string name;

    static std::vector<Client*> clients;
	static Client* GetClient(const CommandInfo& info);
	static Client* GetClient(const std::string& name);

    static StringToFunctioMap commandMap;

	typedef std::pair<std::string, std::string> stringPair;
	static std::unordered_map<std::string, std::string> functionHelpMap;

    static void InterpretCommand(const CommandInfo& info);

};

