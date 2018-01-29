#ifndef _BASE_LOGGER_SINK_H_
#define _BASE_LOGGER_SINK_H_

#include <memory>
#include <string>
#include <cstddef>

#include <core/logger/log_entry.hpp>

namespace core
{

// Abstract base class for sinks with some concrete elements
class BaseLoggerSink
{
    public:
        BaseLoggerSink()
        : m_rotationId{1}
        {
        }

        std::size_t getRotationSize() const
        {
            return m_rotationSize;
        }

        void setRotationSize(std::size_t size)
        {
            m_rotationSize = size;
        }

        void setResourceName(const std::string& name)
        {
            m_resourceName = name;
        }

        //Abstract methods
        virtual void open() = 0;
        virtual void process(LogEntry entry) = 0;
        virtual void close() = 0;

        //Virtual methods
        virtual void rotate()
        {
            m_rotationId++;
        }

    protected:
        std::size_t m_rotationSize;
        unsigned int m_rotationId;
        std::string m_resourceName;
};

using LoggerSinkPtr = std::unique_ptr<BaseLoggerSink>;

}

#endif