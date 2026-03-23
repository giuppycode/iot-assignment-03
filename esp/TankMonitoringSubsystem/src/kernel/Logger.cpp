#include "Logger.h"
#include "MsgService.h"

LoggerService Logger;

void LoggerService::log(const String &msg)
{
    MsgService.sendMsg("lo:" + msg);
}
