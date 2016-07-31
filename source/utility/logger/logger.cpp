#include "logger.h"
using namespace std;

namespace utility
{

void Logger::initialise(size_t bufferSize)
{
    assert(bufferSize>0);
    m_buffer.reset(new concurrent::RingBufferMPMC<LogEntry>(bufferSize));
}

void Logger::log(const LogLevel level, const string& sender, const string& message)
{
    LogEntry entry(level, sender, message);
    pushLogToLogBuffer(entry);
}

void Logger::logForExclusiveSink(const LogLevel level, const std::string& sender, const std::string& message, const std::string& exclusiveSink)
{
    LogEntry entry(level, sender, message, exclusiveSink);
    pushLogToLogBuffer(entry);
}

void Logger::pushLogToLogBuffer(const LogEntry& log)
{
    m_buffer->push(log);
}

bool Logger::processLogs()
{
    bool processedAny = false;
    while(m_buffer->count() > 0)
    {
        processedAny = true;
        auto log = m_buffer->pop();
        m_sinks.processEnabledSinks(log);
    }
    return processedAny;
}

void* Logger::run()
{
    m_sinks.openEnabledSinks();

    while ( true )
    {
        if ( m_sinks.noSinksEnabled() == false )
        {
            processLogs();

            if (isFinishing() == true )
            {
                processLogs();
                break;
            }

        }
        else
        {
            concurrent::Thread::yield();
        }
    }

    m_sinks.closeEnabledSinks();
    return nullptr;
}

}//namespace