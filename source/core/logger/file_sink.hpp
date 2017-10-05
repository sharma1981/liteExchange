#ifndef _FILE_SINK_H_
#define _FILE_SINK_H_

#include <cassert>
#include <fstream>

#include <core/string_utility.h>
#include <core/logger/log_entry.hpp>

#include <core/logger/base_logger_sink.hpp>
#include <core/pretty_exception.h>

namespace core
{

#define FILE_SINK "FILE_SINK"

class FileSink : public BaseLoggerSink
{
    public:

        FileSink() : BaseLoggerSink(FILE_SINK, true)
        {
        }

        void open() override
        {
            assert(m_resourceName.length() > 0);

            if (m_logFile.is_open())
            {
                m_logFile.close();
            }

            m_logFile.open(m_resourceName);

            if (!m_logFile.is_open())
            {
                THROW_PRETTY_RUNTIME_EXCEPTION(core::format("Log file %s can`t be opened", m_resourceName.c_str()))
            }
        }

        void close() override
        {
            m_logFile.close();
        }

        void process(LogEntry entry)
        {
            m_logFile << entry << std::endl;
        }

    private:
        std::ofstream m_logFile;
};

} // namespace

#endif