#ifndef _LOGGER_SINKS_H_
#define _LOGGER_SINKS_H_

#include <utility>
#include <memory>
#include <unordered_map>

#include <utility/logger/base_logger_sink.hpp>
#include <utility/logger/logger_sink_factory.hpp>

namespace utility
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

        void setSinkResourceName(const std::string& sinkName, const std::string& resourceName)
        {
            auto sink = m_sinks.find(sinkName);
            if (sink != m_sinks.end())
            {
                sink->second->setResourceName(resourceName);
            }
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
            auto exclusiveSink = logEntry.getExclusiveSink();
            if (exclusiveSink.length() == 0)
            {
                // No exclusive sink for this log, so send it to all sinks
                for (auto& sink : m_sinks)
                {
                    if (sink.second->enabled())
                    {
                        sink.second->process(logEntry);
                    }
                }
            }
            else
            {
                // Send this log to only one exclusive sink
                auto sink = m_sinks.find(exclusiveSink);
                if (sink != m_sinks.end())
                {
                    sink->second->process(logEntry);
                }
            }
        }

        void closeEnabledSinks()
        {
            for (auto& sink : m_sinks)
            {
                if (sink.second->enabled())
                {
                    sink.second->open();
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