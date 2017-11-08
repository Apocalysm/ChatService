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

/* Key Commands */
const std::string CONNECTTOSERVER_KEY = "/connect";
const std::string DISCONNECTFROMSERVER_KEY = "/disconnect";
const std::string WHISPER_KEY = "/whisper";
const std::string HELP_KEY = "/help";
const std::string CHANGENAME_KEY = "/name";


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
