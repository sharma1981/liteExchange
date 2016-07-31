#ifndef _FILE_SINK_H_
#define _FILE_SINK_H_

#include <cassert>
#include <fstream>
#include <boost/format.hpp>
#include <utility/logger/log_entry.hpp>

#include <utility/logger/base_logger_sink.hpp>
#include <utility/pretty_exception.h>

namespace utility
{

#define FILE_SINK "FILE_SINK"

class FileSink : public BaseLoggerSink
{
    public:

        FileSink() : BaseLoggerSink(FILE_SINK, true)
        {
        }

        void open() throw(std::runtime_error) override
        {
            assert(m_resourceName.length() > 0);

            if (m_logFile.is_open())
            {
                m_logFile.close();
            }

            m_logFile.open(m_resourceName);

            if (!m_logFile.is_open())
            {
                THROW_PRETTY_RUNTIME_EXCEPTION(boost::str(boost::format("Log file %s can`t be opened") % m_resourceName.c_str()))
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