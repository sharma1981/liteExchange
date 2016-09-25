#ifndef _SERVER_CONFIGURATION_H_
#define _SERVER_CONFIGURATION_H_

#include <vector>
#include <string>
#include <exception>

#include <server/server_constants.h>

#include <utility/config_file.h>
#include <utility/logger/logger_sink_factory.hpp>
#include <utility/os_utility.h>

#include <concurrent/thread_pool_arguments.h>
#include <concurrent/thread_priority.h>

class ServerConfiguration
{
    public :

        ServerConfiguration()
        : m_singleInstanceTCPPortNumber{ 0 }
        {
        }

        void load(const std::string& configurationFile)
        {
            utility::ConfigFile configuration;
            configuration.loadFromFile(configurationFile);

            m_singleInstanceTCPPortNumber = configuration.getIntValue(server_constants::CONFIGURATION_FILE_SINGLE_INSTANCE_TCP_PORT);

            // Get logging related configurations, the loop is for per logger sink
            utility::Logger::getInstance()->initialise(configuration.getIntValue(server_constants::CONFIGURATION_FILE_LOG_BUFFER_SIZE));

            for (auto& sinkName : utility::LOGGER_SINKS)
            {
                if (configuration.getBoolValue(sinkName) == true)
                {
                    utility::Logger::getInstance()->setSinkEnabled(sinkName, true);

                    std::string resourceNameAttribute = std::string(sinkName) + "_RESOURCE_NAME";
                    if (configuration.doesAttributeExist(resourceNameAttribute))
                    {
                        auto resourceName = configuration.getStringValue(resourceNameAttribute);
                        utility::Logger::getInstance()->setSinkResourceName(sinkName, resourceName);
                    }
                }
            }

            m_symbols = configuration.getArray(server_constants::CONFIGURATION_FILE_SYMBOL_ARAY);
            if (m_symbols.size() == 0)
            {
                throw std::runtime_error("No symbol found in the ini file");
            }

            m_threadPoolArguments.m_pinThreadsToCores = configuration.getBoolValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_PIN_THREADS_TO_CORES);
            m_threadPoolArguments.m_hyperThreading = configuration.getBoolValue(server_constants::CONFIGURATION_FILE_HYPER_THREADING);
            m_threadPoolArguments.m_workQueueSizePerThread = configuration.getIntValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_WORK_QUEUE_SIZE_PER_THREAD);
            m_threadPoolArguments.m_threadStackSize = configuration.getIntValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_THREAD_STACK_SIZE);
            m_threadPoolArguments.m_threadPriority = concurrent::getThreadPriorityFromString(configuration.getStringValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_THREAD_PRIORITY));
        }

        void setSingleInstancePortNumber(int singleInstancePortNumber) { m_singleInstanceTCPPortNumber = singleInstancePortNumber; }
        int getSingleInstancePortNumber() const{ return m_singleInstanceTCPPortNumber; }

        void setThreadPoolArguments(const concurrent::ThreadPoolArguments& threadPoolArguments) { m_threadPoolArguments = threadPoolArguments; }
        concurrent::ThreadPoolArguments getThreadPoolArguments() const { return m_threadPoolArguments; }

        void setSymbols(const std::vector<std::string>& symbols) { m_symbols = symbols; }
        std::vector<std::string> getSymbols() const { return m_symbols;  }

    private :
        int m_singleInstanceTCPPortNumber;
        concurrent::ThreadPoolArguments m_threadPoolArguments;
        std::vector<std::string> m_symbols;
};


#endif