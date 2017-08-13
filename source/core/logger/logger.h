#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <cstddef>
#include <string>

#include <concurrent/lock.hpp>
#include <concurrent/actor.h>
#include <concurrent/ring_buffer_mpmc.hpp>

#include <core/design_patterns/singleton_dclp.hpp>

#include <core/logger/log_levels.h>
#include <core/logger/log_entry.hpp>
#include <core/logger/logger_sinks.hpp>

namespace core
{

#define LOG_INFO(SENDER, MESSAGE) (core::Logger::getInstance()->log(core::LogLevel::LEVEL_INFO,(SENDER),(MESSAGE)));
#define LOG_WARNING(SENDER, MESSAGE) (core::Logger::getInstance()->log(core::LogLevel::LEVEL_WARNING,(SENDER),(MESSAGE)));
#define LOG_ERROR(SENDER, MESSAGE) (core::Logger::getInstance()->log(core::LogLevel::LEVEL_ERROR,(SENDER),(MESSAGE)));
//Sink-exclusive macros, the logs will be sent to only specified sinks
#define LOG_CONSOLE(SENDER, MESSAGE) (core::Logger::getInstance()->logForExclusiveSink(core::LogLevel::LEVEL_INFO,(SENDER),(MESSAGE),(CONSOLE_SINK)));
#define LOG_FILE(SENDER, MESSAGE) (core::Logger::getInstance()->logForExclusiveSink(core::LogLevel::LEVEL_INFO,(SENDER),(MESSAGE),(FILE_SINK)));

#define DEFAULT_LOGGER_RING_BUFFER_SIZE 819200

using LogBuffer = std::unique_ptr< concurrent::RingBufferMPMC<LogEntry> >;

class Logger : public concurrent::Actor, public SingletonDCLP<Logger>
{
    public :
        Logger() : Actor("LoggerThread") {}
        ~Logger() { shutdown(); }

        void initialise(std::size_t bufferSize = DEFAULT_LOGGER_RING_BUFFER_SIZE);

        void setSinkEnabled(const std::string& sinkName, bool value)
        {
            m_sinks.setSinkEnabled(sinkName, value);
        }

        void setSinkResourceName(const std::string& sinkName, const std::string& resourceName)
        {
            m_sinks.setSinkResourceName(sinkName, resourceName);
        }

        void log(const LogLevel level, const std::string& sender, const std::string& message);
        void logForExclusiveSink(const LogLevel level, const std::string& sender, const std::string& message, const std::string& exclusiveSink);
        void* run() override;

    private:
        void pushLogToLogBuffer(const LogEntry& log);
        // Move ctor deletion
        Logger(Logger&& other) = delete;
        // Move assignment operator deletion
        Logger& operator=(Logger&& other) = delete;
        bool processLogs();
        LogBuffer m_buffer;
        LoggerSinks m_sinks;
};

}//namespace
#endif