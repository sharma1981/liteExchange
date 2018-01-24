#ifndef _LOG_LEVELS_
#define _LOG_LEVELS_

#include <string>

namespace core
{

enum class LogLevel { LEVEL_INFO, LEVEL_WARNING, LEVEL_ERROR };

inline void logLevelToString(LogLevel level, std::string& outputString)
{
    switch (level)
    {
        case LogLevel::LEVEL_INFO:
            outputString = "INFO";
            break;

        case LogLevel::LEVEL_WARNING:
            outputString = "WARNING";
            break;

        case LogLevel::LEVEL_ERROR:
            outputString = "ERROR";
            break;
    }
}

}

#endif