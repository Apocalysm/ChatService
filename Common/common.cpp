#include "Common.h"

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
