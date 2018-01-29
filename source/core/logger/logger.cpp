#include <utility>
#include <algorithm>
#include <type_traits>

#include "logger.h"

#include <core/logger/console_sink.hpp>
#include <core/logger/memory_mapped_file_sink.hpp>

using namespace std;

namespace core
{

void Logger::initialise(const LoggerArguments& configuration)
{
    // Disc write period
    m_writePeriodInMicroseconds = configuration.m_writePeriodInMilliSeconds * 1000;

    // Ring buffer size
    m_buffer.reset(new core::RingBufferMPMC<LogEntry>(configuration.m_bufferSize));

    // Log level
    m_logLevel = static_cast<std::underlying_type<LogLevel>::type >(configuration.m_logLevel);

    // Memory mapped file sink
    if (configuration.m_memoryMappedFileName.length() > 0)
    {
        auto memoryMappedFileSink = new MemoryMappedFileSink();
        memoryMappedFileSink->setResourceName(configuration.m_memoryMappedFileName);
        memoryMappedFileSink->setRotationSize(configuration.m_rotationSizeInBytes);
        m_sinks.emplace_back(std::move(memoryMappedFileSink));
    }

    // Console sink if specified
    if (configuration.m_copyToStdout)
    {
        m_sinks.emplace_back(std::move(new  ConsoleSink));
    }
}

void Logger::log(const LogLevel level, const string& sender, const string& message, const string& sourceCode, const string& sourceCodeLineNumber)
{
    auto logLevel = static_cast<std::underlying_type<LogLevel>::type >(level);

    if (m_logLevel >= logLevel)
    {
        LogEntry entry(level, sender, message, sourceCode, sourceCodeLineNumber);
        pushLogToLogBuffer(entry);
    }
}

void Logger::pushLogToLogBuffer(const LogEntry& log)
{
    m_buffer->push(log);
}

void* Logger::run()
{
    // Open all sinks
    std::for_each(m_sinks.begin(), m_sinks.end(), [](std::unique_ptr<BaseLoggerSink>& sink){sink->open(); });

    while ( true )
    {
        while (m_buffer->count() > 0)
        {
            auto log = m_buffer->pop();
            // Process all sinks
            std::for_each(m_sinks.begin(), m_sinks.end(), [&log](std::unique_ptr<BaseLoggerSink>& sink){sink->process(log); });
        }

        if (isFinishing() == true)
        {
            break;
        }

        applyWaitStrategy(m_writePeriodInMicroseconds);
    }

    // Close all sinks
    std::for_each(m_sinks.begin(), m_sinks.end(), [](std::unique_ptr<BaseLoggerSink>& sink){sink->close(); });

    return nullptr;
}

}//namespace