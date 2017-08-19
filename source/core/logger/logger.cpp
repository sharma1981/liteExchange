#include "logger.h"
using namespace std;

namespace core
{

void Logger::initialise(size_t bufferSize)
{
    assert(bufferSize>0);
    m_buffer.reset(new core::RingBufferMPMC<LogEntry>(bufferSize));
}

void Logger::log(const LogLevel level, const string& sender, const string& message, const string& sourceCode, const string& sourceCodeLineNumber)
{
    LogEntry entry(level, sender, message, sourceCode, sourceCodeLineNumber);
    pushLogToLogBuffer(entry);
}

void Logger::logForExclusiveSink(const LogLevel level, const string& sender, const string& message, const string& sourceCode, const string& sourceCodeLineNumber, const string& exclusiveSink)
{
    LogEntry entry(level, sender, message, sourceCode,  sourceCodeLineNumber, exclusiveSink);
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
            core::Thread::yield();
        }
    }

    m_sinks.closeEnabledSinks();
    return nullptr;
}

}//namespace