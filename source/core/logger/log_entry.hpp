#ifndef _LOG_ENTRY_
#define _LOG_ENTRY_

#include <string>
#include <iostream>
#include <sstream>

#include <core/logger/log_levels.h>
#include <core/datetime_utility.h>
#include <core/string_utility.h>

namespace core
{

class LogEntry
{
    public :

    LogEntry()
    {
    };

    LogEntry(const LogLevel level, const std::string sender, const std::string message, const std::string sourceCode, const std::string sourceCodeLineNumber)
        : m_logLevel(level), m_sender(sender), m_message(message), m_sourceCode(sourceCode), m_sourceCodeLineNumber(sourceCodeLineNumber)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, LogEntry& entry)
    {
        std::string logLevel;
        logLevelToString(entry.m_logLevel, logLevel);

        os << core::format("[ %s : %s ]",  entry.m_sourceCode, entry.m_sourceCodeLineNumber);
        os << std::endl;
        os << core::format("%s : %s , %s , %s", core::getUtcDatetime(core::Subseconds::MICROSECONDS), logLevel, entry.m_sender, entry.m_message);
        return os;
    }

    const std::string toString() const
    {
        std::stringstream stream;
        stream << this;
        return stream.str();
    }

    const std::string& getMessage() const
    {
        return m_message;
    }

    private :
        LogLevel m_logLevel;
        std::string m_sender;
        std::string m_message;
        std::string m_sourceCode;
        std::string m_sourceCodeLineNumber;
};

}

#endif