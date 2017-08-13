#ifndef _SERVER_CONFIGURATION_H_
#define _SERVER_CONFIGURATION_H_

#include <vector>
#include <string>

#include <server/server_constants.h>

#include <core/config_file.h>
#include <core/logger/logger_sink_factory.hpp>
#include <core/os_utility.h>

#include <core/concurrent/thread_pool_arguments.h>
#include <core/concurrent/thread_priority.h>

class ServerConfiguration
{
    public :

        ServerConfiguration()
        : m_singleInstanceTCPPortNumber{ 0 }, m_isMatchingMultithreaded{ false }
        {
        }

        void load(const std::string& configurationFile)
        {
            core::ConfigFile configuration;
            configuration.loadFromFile(configurationFile);

            m_singleInstanceTCPPortNumber = configuration.getIntValue(server_constants::CONFIGURATION_FILE_SINGLE_INSTANCE_TCP_PORT);

            // Get logging related configurations, the loop is for per logger sink
            core::Logger::getInstance()->initialise(configuration.getIntValue(server_constants::CONFIGURATION_FILE_LOGGER_BUFFER_SIZE));

            for (auto& sinkName : core::LOGGER_SINKS)
            {
                std::string configSinkName = "LOGGER_" + std::string(sinkName);
                if (configuration.doesAttributeExist(configSinkName) == true)
                {
                    if (configuration.getBoolValue(configSinkName) == true)
                    {
                        core::Logger::getInstance()->setSinkEnabled(sinkName, true);

                        std::string resourceNameAttribute = "LOGGER_" + std::string(sinkName) + "_RESOURCE_NAME";
                        if (configuration.doesAttributeExist(resourceNameAttribute))
                        {
                            auto resourceName = configuration.getStringValue(resourceNameAttribute);
                            core::Logger::getInstance()->setSinkResourceName(sinkName, resourceName);
                        }
                    }
                }
            }

            // Get symbol configuration
            m_symbols = configuration.getArray(server_constants::CONFIGURATION_FILE_SYMBOL_ARAY);
            if (m_symbols.size() == 0)
            {
                throw std::runtime_error("No symbol found in the ini file");
            }

            // Get matching multithreading mode
            m_isMatchingMultithreaded = configuration.getBoolValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_MULTITHREADED_ORDER_MATCHING);

            // Get multithreading configuration
            if (m_isMatchingMultithreaded)
            {
                m_threadPoolArguments.m_pinThreadsToCores = configuration.getBoolValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_PIN_THREADS_TO_CORES);
                m_threadPoolArguments.m_hyperThreading = configuration.getBoolValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_HYPER_THREADING);
                m_threadPoolArguments.m_workQueueSizePerThread = configuration.getIntValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_WORK_QUEUE_SIZE_PER_THREAD);
                m_threadPoolArguments.m_threadStackSize = configuration.getIntValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_THREAD_STACK_SIZE);
                m_threadPoolArguments.m_threadPriority = core::getThreadPriorityFromString(configuration.getStringValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_THREAD_PRIORITY));
            }
        }

        int getSingleInstancePortNumber() const{ return m_singleInstanceTCPPortNumber; }
        bool getMatchingMultithreadingMode() const { return m_isMatchingMultithreaded; }
        core::ThreadPoolArguments getThreadPoolArguments() const { return m_threadPoolArguments; }
        std::vector<std::string> getSymbols() const { return m_symbols;  }

    private :
        int m_singleInstanceTCPPortNumber;
        bool m_isMatchingMultithreaded;
        core::ThreadPoolArguments m_threadPoolArguments;
        std::vector<std::string> m_symbols;
};


#endif