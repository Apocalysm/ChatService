#include "Common.h"

CommandKey::CommandKey(const char* key_, const char* help_)
    : key(key_)
    , help(help_)
{
}

const std::string& CommandKey::Key() const
{
    return key;
}

const std::string& CommandKey::Help() const
{
    return help;
}

CommandInfo::CommandInfo()
{

}

CommandInfo::CommandInfo(const std::string& bfr, size_t bfr_size, QHostAddress ip, unsigned short prt) :
    buffer(bfr),
    size(bfr_size),
    address(ip),
    port(prt)
{
}

CommandInfo::~CommandInfo()
{
    //delete address;
    //delete buffer;
}
