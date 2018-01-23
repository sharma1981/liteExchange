#ifndef _SHARED_MEMORY_SINK_H_
#define _SHARED_MEMORY_SINK_H_

#include <cassert>
#include <sstream>
#include <string>

#include <core/logger/log_entry.hpp>
#include <core/logger/base_logger_sink.hpp>

#include <core/pretty_exception.h>
#include <core/shared_memory.h>
#include <core/string_utility.h>

namespace core
{

#define SHARED_MEMORY_SINK "SHARED_MEMORY_SINK"
#define SHARED_MEMORY_SINK_SIZE 10240000

    class SharedMemorySink : public BaseLoggerSink
    {
    public:

        SharedMemorySink() : BaseLoggerSink(SHARED_MEMORY_SINK, true)
        {
        }

        void open() override
        {
            assert(m_resourceName.length() > 0);

            m_logFile.open(m_resourceName, SHARED_MEMORY_SINK_SIZE, true);

            if (!m_logFile.isOpen())
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
            std::stringstream stream;
            stream << entry << std::endl;
            auto entryAsString = stream.str();
            m_logFile.write(static_cast<void *>(&entryAsString[0]), entryAsString.length());
        }

    private:
        core::SharedMemory m_logFile;
    };

} // namespace

#endif