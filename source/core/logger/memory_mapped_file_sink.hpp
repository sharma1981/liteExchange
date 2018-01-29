#ifndef _MEMORY_MAPPED_FILE_SINK_H_
#define _MEMORY_MAPPED_FILE_SINK_H_

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

class MemoryMappedFileSink : public BaseLoggerSink
{
    public:

        MemoryMappedFileSink() : BaseLoggerSink()
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

            m_logFile.append(static_cast<void *>(&entryAsString[0]), entryAsString.length());
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
            m_logFile.open(m_resourceName, m_rotationSize, true);

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