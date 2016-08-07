#ifndef _CONSOLE_SINK_H_
#define _CONSOLE_SINK_H_

#include <utility/logger/base_logger_sink.hpp>
#include <utility/logger/log_entry.hpp>
#include <iostream>

namespace utility
{

#define CONSOLE_SINK "CONSOLE_SINK"

class ConsoleSink : public BaseLoggerSink
{
    public:

        ConsoleSink() : BaseLoggerSink(CONSOLE_SINK, false)
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