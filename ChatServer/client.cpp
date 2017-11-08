#include "Client.h"


Client::Client(const QHostAddress& address, const unsigned short port, const std::string& name) :
    m_address(address),
    m_port(port),
    m_name(name)
{
}


Client::~Client()
{
}

const QHostAddress Client::GetAddress() const
{
    return m_address;
}

const unsigned short Client::GetPort() const
{
    return m_port;
}

const std::string& Client::GetName() const
{
    return m_name;
}

void Client::SetName(const std::string& newName)
{
    m_name = newName;
}
