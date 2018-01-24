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
#define DEFAULT_SHARED_MEMORY_SINK_SIZE 10240000

class SharedMemorySink : public BaseLoggerSink
{
    public:

        SharedMemorySink() : BaseLoggerSink(SHARED_MEMORY_SINK, true)
        {
        }

        void open() override
        {
            assert(m_resourceName.length() > 0);

            if (core::contains(m_resourceName, "."))
            {
                auto resourceNameParts = core::split(m_resourceName, '.');
                m_originalResourceName = resourceNameParts[0];
                m_resourceExtension = resourceNameParts[1];
            }
            else
            {
                m_originalResourceName = m_resourceName;
            }

            if( m_resourceSize == 0)
            {
                m_resourceSize = DEFAULT_SHARED_MEMORY_SINK_SIZE;
            }

            openMemoryMappedFile();
        }

        void close() override
        {
            closeMemoryMappedFile();
        }

        void process(LogEntry entry)
        {
            std::stringstream stream;
            stream << entry << std::endl;
            auto entryAsString = stream.str();

            if (m_logFile.getSize() <= m_logFile.getWrittenSize() + entryAsString.length())
            {
                rotate();
            }

            m_logFile.write(static_cast<void *>(&entryAsString[0]), entryAsString.length());
        }

        void rotate() override
        {
            BaseLoggerSink::rotate();

            m_resourceName = m_originalResourceName + "_" + std::to_string(m_rotationId);

            if (m_resourceExtension.length() > 0)
            {
                m_resourceName += "." + m_resourceExtension;
            }

            closeMemoryMappedFile();
            openMemoryMappedFile();
        }

    private:
        std::string m_originalResourceName;
        std::string m_resourceExtension;
        core::SharedMemory m_logFile;

        void openMemoryMappedFile()
        {
            m_logFile.open(m_resourceName, m_resourceSize, true);

            if (!m_logFile.isOpen())
            {
                THROW_PRETTY_RUNTIME_EXCEPTION(core::format("Log file %s can`t be opened", m_resourceName.c_str()))
            }
        }

        void closeMemoryMappedFile()
        {
            m_logFile.close();
        }
};

} // namespace

#endif