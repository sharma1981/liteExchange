#ifndef _CONSOLE_SINK_H_
#define _CONSOLE_SINK_H_

#include <core/logger/base_logger_sink.hpp>
#include <core/logger/log_entry.hpp>
#include <iostream>

namespace core
{

class ConsoleSink : public BaseLoggerSink
{
    public:

        ConsoleSink() : BaseLoggerSink()
        {
        }

        void open() override
        {
        }

        void close() override
        {
        }

        void process(LogEntry entry)
        {
            std::cout << entry.getMessage() << std::endl;
        }
};

} // namespace

#endif