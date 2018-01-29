#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <cstddef>
#include <string>
#include <memory>
#include <vector>

#include <core/concurrency/actor.h>
#include <core/concurrency/thread_wait_strategy.h>
#include <core/concurrency/ring_buffer_mpmc.hpp>

#include <core/design_patterns/singleton_dclp.hpp>

#include <core/logger/logger_arguments.h>
#include <core/logger/log_levels.h>
#include <core/logger/log_entry.hpp>
#include <core/logger/base_logger_sink.hpp>

namespace core
{

#define STRINGIFY_DETAIL(x) #x
#define STRINGIFY(x) STRINGIFY_DETAIL(x)

#define LOG_INFO(SENDER, MESSAGE) (core::Logger::getInstance()->log(core::LogLevel::LEVEL_INFO,(SENDER),(MESSAGE),(__FILE__),(STRINGIFY(__LINE__))));
#define LOG_WARNING(SENDER, MESSAGE) (core::Logger::getInstance()->log(core::LogLevel::LEVEL_WARNING,(SENDER),(MESSAGE),(__FILE__),(STRINGIFY(__LINE__))));
#define LOG_ERROR(SENDER, MESSAGE) (core::Logger::getInstance()->log(core::LogLevel::LEVEL_ERROR,(SENDER),(MESSAGE),(__FILE__),(STRINGIFY(__LINE__))));

using LogBuffer = std::unique_ptr< core::RingBufferMPMC<LogEntry> >;

class Logger : public core::Actor, public SingletonDCLP<Logger>, public SleepWaitStrategy
{
    public :
        Logger() : Actor("LoggerThread") {}
        ~Logger() { shutdown(); }

        void initialise(const LoggerArguments&);
        void log(const LogLevel level, const std::string& sender, const std::string& message, const std::string& sourceCode, const std::string& sourceCodeLineNumber);
        void* run() override;

    private:
        void pushLogToLogBuffer(const LogEntry& log);

        // Move ctor deletion
        Logger(Logger&& other) = delete;
        // Move assignment operator deletion
        Logger& operator=(Logger&& other) = delete;

        LogBuffer m_buffer;
        std::vector<std::unique_ptr<BaseLoggerSink>> m_sinks;
        std::size_t m_writePeriodInMicroseconds = DEFAULT_LOGGER_WRITE_PERIOD_MILLISECONDS*1000;
        int m_logLevel = 0;
};

}//namespace
#endif