#include "logger.h"
using namespace std;

namespace core
{

void Logger::initialise(const core::Configuration& configuration)
{
    // LOGGER BUFFER SIZE
    auto bufferSize = configuration.getIntValue("LOGGER_BUFFER_SIZE");
    if (bufferSize == 0)
    {
        bufferSize = DEFAULT_LOGGER_RING_BUFFER_SIZE;
    }

    m_buffer.reset(new core::RingBufferMPMC<LogEntry>(bufferSize));

    // LOGGER SINKS
    for (auto& sinkName : LOGGER_SINKS)
    {
        std::string configSinkName = "LOGGER_" + std::string(sinkName);
        if (configuration.doesAttributeExist(configSinkName) == true)
        {
            if (configuration.getBoolValue(configSinkName) == true)
            {
                auto sink = m_sinks.getSink(sinkName);

                // ENABLE SINK
                sink->setEnabled(true);

                // SET SINK RESOURCE NAME
                std::string resourceNameAttribute = "LOGGER_" + std::string(sinkName) + "_RESOURCE_NAME";
                if (configuration.doesAttributeExist(resourceNameAttribute))
                {
                    auto resourceName = configuration.getStringValue(resourceNameAttribute);
                    sink->setResourceName(resourceName);
                }

                // SET SINK SIZE
                std::string resourceSizeAttribute = "LOGGER_" + std::string(sinkName) + "_RESOURCE_SIZE";
                if (configuration.doesAttributeExist(resourceSizeAttribute))
                {
                    auto resourceSize = configuration.getIntValue(resourceSizeAttribute);
                    sink->setSize(resourceSize);
                }
            }
        }
    }
}

void Logger::log(const LogLevel level, const string& sender, const string& message, const string& sourceCode, const string& sourceCodeLineNumber)
{
    if (m_sinks.noSinksEnabled() == false)
    {
        LogEntry entry(level, sender, message, sourceCode, sourceCodeLineNumber);
        pushLogToLogBuffer(entry);
    }
}

void Logger::pushLogToLogBuffer(const LogEntry& log)
{
    m_buffer->push(log);
}

bool Logger::processLogs()
{
    bool processedAny{ false };

    if (m_sinks.noSinksEnabled() == false)
    {

        while (m_buffer->count() > 0)
        {
            processedAny = true;
            auto log = m_buffer->pop();
            m_sinks.processEnabledSinks(log);
        }
    }

    return processedAny;
}

void* Logger::run()
{
    m_sinks.openEnabledSinks();

    while ( true )
    {
        processLogs();

        if (isFinishing() == true)
        {
            break;
        }

        applyWaitStrategy(1000); // 50 milliseconds wait
    }

    m_sinks.closeEnabledSinks();
    return nullptr;
}

}//namespace