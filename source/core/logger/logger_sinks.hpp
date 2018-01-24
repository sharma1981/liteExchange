#ifndef _LOGGER_SINKS_H_
#define _LOGGER_SINKS_H_

#include <utility>
#include <memory>
#include <unordered_map>

#include <core/logger/base_logger_sink.hpp>
#include <core/logger/logger_sink_factory.hpp>

namespace core
{

class LoggerSinks
{
    public :

        LoggerSinks()
        {
            m_sinks.reserve(SINK_NUMBER);
            addSinks();
        }

        ~LoggerSinks()
        {
            closeEnabledSinks();
        }

        BaseLoggerSink* getSink(const std::string& sinkName)
        {
            auto sink = m_sinks.find(sinkName);
            if (sink != m_sinks.end())
            {
                return sink->second.get();
            }
            return nullptr;
        }

        void setSinkEnabled(const std::string& sinkName, bool value)
        {
            auto sink = m_sinks.find(sinkName);
            if (sink != m_sinks.end())
            {
                sink->second->setEnabled(value);
            }
        }

        bool noSinksEnabled() const
        {
            for (auto& sink : m_sinks)
            {
                if (sink.second->enabled())
                {
                    return false;
                }
            }
            return true;
        }

        void openEnabledSinks()
        {
            for (auto& sink : m_sinks)
            {
                if (sink.second->enabled())
                {
                    sink.second->open();
                }
            }
        }

        void processEnabledSinks(const LogEntry& logEntry)
        {
            for (auto& sink : m_sinks)
            {
                if (sink.second->enabled())
                {
                    sink.second->process(logEntry);
                }
            }
        }

        void closeEnabledSinks()
        {
            for (auto& sink : m_sinks)
            {
                if (sink.second->enabled())
                {
                    sink.second->close();
                }
            }
        }

    private :
        std::unordered_map<std::string, LoggerSinkPtr> m_sinks;

        void addSinks()
        {
            LoggerSinkFactory sinkFactory;
            for (auto& sinkName : LOGGER_SINKS)
            {
                auto sink = sinkFactory.create(sinkName);
                m_sinks.insert(std::make_pair(sinkName, std::move(sink)));
            }
        }
};



} // namespace

#endif