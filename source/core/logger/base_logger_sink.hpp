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
        BaseLoggerSink(const std::string& sinkName, bool requiresResource)
        : m_enabled{ false }, m_requiresResourceName{ requiresResource }, m_sinkName(sinkName), m_resourceSize{ 0 }, m_rotationId{1}
        {
        }

        void setEnabled(bool value)
        {
            m_enabled = value;
        }

        bool enabled() const
        {
            return m_enabled;
        }

        const std::string& getSinkName() const
        {
            return m_sinkName;
        }

        void setResourceName(const std::string& resourceName)
        {
            m_resourceName = resourceName;
        }

        bool requiresResourceName() const
        {
            return m_requiresResourceName;
        }

        std::size_t getSize() const
        {
            return m_resourceSize;
        }

        void setSize(std::size_t size)
        {
            m_resourceSize = size;
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

    private:
        bool m_enabled;
        bool m_requiresResourceName ;
        std::string m_sinkName;


    protected:
        std::size_t m_resourceSize;
        unsigned int m_rotationId;
        std::string m_resourceName;
};

using LoggerSinkPtr = std::unique_ptr<BaseLoggerSink>;

}

#endif