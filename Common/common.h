#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <QtNetwork/QHostAddress>



#ifdef COMMON
 #define COMMON_DLLSPEC Q_DECL_EXPORT
#else
 #define COMMON_DLLSPEC Q_DECL_IMPORT
#endif

const unsigned short SERVER_PORT = 55002;
struct CommandKey
{
    const COMMON_DLLSPEC std::string& Key() const;
    const COMMON_DLLSPEC std::string& Help() const;

    COMMON_DLLSPEC CommandKey(const char* key_, const char* help_);

private:
	std::string key;
	std::string help;
};

/* Key Commands */
const CommandKey CMD_CONNECTTOSERVER = { "/connect", "" };
const CommandKey CMD_DISCONNECTFROMSERVER = { "/disconnect", "" };
const CommandKey CMD_WHISPER = { "/whisper", "/whisper &lt;name&gt;" };
const CommandKey CMD_CHANGENAME = { "/name", "/name &lt;NewName&gt;" };
const CommandKey CMD_HELP = { "/help", "" };


struct CommandInfo
{
public:

    COMMON_DLLSPEC CommandInfo();
    COMMON_DLLSPEC CommandInfo(const std::string& bfr, size_t bfr_size, QHostAddress ip, unsigned short prt) ;

    COMMON_DLLSPEC ~CommandInfo();

    std::string buffer;
    size_t size;
    QHostAddress address;
    unsigned short port;

    std::string args;
};


typedef std::function<void(const CommandInfo&)> commandFunction;
typedef std::pair<std::string, commandFunction> commandPair;
typedef std::unordered_map<std::string, commandFunction> StringToFunctioMap;
