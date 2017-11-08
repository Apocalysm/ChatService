#pragma once
#include <string>

#include <QtNetwork/QHostAddress>

// Client class is the observer in an observer pattern with the Server class as subject
class Client
{
public:
    Client(const QHostAddress& address, const unsigned short port, const std::string& name);
    ~Client();

    const QHostAddress GetAddress() const;
    const unsigned short GetPort() const;

    const std::string& GetName() const;
    void SetName(const std::string& newName);

private:
    const QHostAddress m_address;
    const unsigned short m_port;

    std::string m_name;
};

