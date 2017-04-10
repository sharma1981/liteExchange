#ifndef _SHARED_MEMORY_SINK_H_
#define _SHARED_MEMORY_SINK_H_

#include <cassert>
#include <sstream>
#include <string>

#include <boost/format.hpp>
#include <utility/logger/log_entry.hpp>

#include <utility/logger/base_logger_sink.hpp>
#include <utility/pretty_exception.h>

#include <memory/shared_memory.h>

namespace utility
{

#define SHARED_MEMORY_SINK "SHARED_MEMORY_SINK"

    class SharedMemorySink : public BaseLoggerSink
    {
    public:

        SharedMemorySink() : BaseLoggerSink(SHARED_MEMORY_SINK, true)
        {
        }

        void open() override
        {
            assert(m_resourceName.length() > 0);

			m_logFile.open(m_resourceName, DEFAULT_VIRTUAL_MEMORY_PAGE_SIZE, true);

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
            std::stringstream stream;
            stream << entry << std::endl;
            auto entryAsString = stream.str();
            m_logFile.write(static_cast<void *>(&entryAsString[0]), entryAsString.length());
        }

    private:
        memory::SharedMemory m_logFile;
    };

} // namespace

#endif