#ifndef _SERVER_CONFIGURATION_H_
#define _SERVER_CONFIGURATION_H_

#include <vector>
#include <string>

#include <server/server_constants.h>

#include <core/configuration.h>
#include <core/logger/logger_sink_factory.hpp>

#include <core/concurrency/thread_pool_arguments.h>
#include <core/concurrency/thread_priority.h>

#include <core/pretty_exception.h>

class ServerConfiguration
{
    public :

        ServerConfiguration()
        : m_singleInstanceTCPPortNumber{ 0 }, m_outgoingMessageQueueSizePerThread{ 0 }, m_isMatchingMultithreaded{ false }
        {
        }

        void load(const std::string& configurationFile)
        {
            core::Configuration configuration;
            core::Configuration::loadFromFile(configurationFile, configuration);

            m_singleInstanceTCPPortNumber = configuration.getIntValue(server_constants::CONFIGURATION_FILE_SINGLE_INSTANCE_TCP_PORT, server_constants::CONFIFURATION_DEFAULT_SINGLE_INSTANCE_TCP_PORT);
            m_processPriority = configuration.getStringValue(server_constants::CONFIGURATION_FILE_PROCESS_PRIORITY, server_constants::CONFIGURATION_DEFAULT_PROCESS_PRIORITY);

            if (configuration.doesAttributeExist(server_constants::CONFIGURATION_FILE_OFFLINE_ORDER_ENTRY_FILE))
            {
                m_offlineOrderEntryFile = configuration.getStringValue(server_constants::CONFIGURATION_FILE_OFFLINE_ORDER_ENTRY_FILE);
            }

            // Get logger configuration
            m_loggerConfiguration = configuration.getSubConfiguration("LOGGER_");

            // Get symbol configuration
            m_symbols = configuration.getArray(server_constants::CONFIGURATION_FILE_SYMBOL_ARRAY);

            if (m_symbols.size() == 0)
            {
                THROW_PRETTY_RUNTIME_EXCEPTION("No symbol found in the ini file");
            }

            // Get matching multithreading mode
            m_isMatchingMultithreaded = configuration.getBoolValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_MULTITHREADED_ORDER_MATCHING, server_constants::CONFIGURATION_DEFAULT_CENTRAL_ORDER_BOOK_MULTITHREADED_ORDER_MATCHING_ENGINE);

            // Get multithreading configuration
            if (m_isMatchingMultithreaded)
            {
                m_threadPoolArguments.m_pinThreadsToCores = configuration.getBoolValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_PIN_THREADS_TO_CORES, server_constants::CONFIGURATION_DEFAULT_CENTRAL_ORDER_BOOK_PIN_THREADS_TO_CORES);
                m_threadPoolArguments.m_hyperThreading = configuration.getBoolValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_HYPER_THREADING, server_constants::CONFIGURATION_DEFAULT_CENTRAL_ORDER_BOOK_HYPER_THREADING);
                m_threadPoolArguments.m_workQueueSizePerThread = configuration.getIntValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_WORK_QUEUE_SIZE_PER_THREAD, server_constants::CONFIGURATION_DEFAULT_CENTRAL_ORDER_BOOK_WORK_QUEUE_SIZE_PER_THREAD);
                m_threadPoolArguments.m_threadStackSize = configuration.getIntValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_THREAD_STACK_SIZE, server_constants::CONFIGURATION_DEFAULT_CENTRAL_ORDER_BOOK_THREAD_STACK_SIZE);
                m_threadPoolArguments.m_threadPriority = core::getThreadPriorityFromString(configuration.getStringValue(server_constants::CONFIGURATION_FILE_CENTRAL_ORDER_BOOK_THREAD_PRIORITY, server_constants::CONFIGURATION_DEFAULT_CENTRAL_ORDER_BOOK_THREAD_PRIORITY));
            }

            // Get outgoing message processor configuration
            m_outgoingMessageQueueSizePerThread = configuration.getIntValue(server_constants::CONFIGURATION_FILE_OUTGOING_MESSAGE_QUEUE_SIZE_PER_THREAD, server_constants::CONFIGURATION_DEFAULT_OUTGOING_MESSAGE_QUEUE_SIZE_PER_THREAD);
        }

        int getSingleInstancePortNumber() const{ return m_singleInstanceTCPPortNumber; }
        int getOutgoingMessageQueueSizePerThread()const { return m_outgoingMessageQueueSizePerThread; }
        std::string getProcessPriority() const { return m_processPriority;  }
        bool getMatchingMultithreadingMode() const { return m_isMatchingMultithreaded; }
        core::ThreadPoolArguments getThreadPoolArguments() const { return m_threadPoolArguments; }
        std::vector<std::string> getSymbols() const { return m_symbols;  }
        std::string getOfflineOrderEntryFile() const { return m_offlineOrderEntryFile; }
        core::Configuration getLoggerConfiguration() const { return m_loggerConfiguration; }

    private :
        int m_singleInstanceTCPPortNumber;
        int m_outgoingMessageQueueSizePerThread;
        std::string m_processPriority;
        std::string m_offlineOrderEntryFile;
        bool m_isMatchingMultithreaded;
        core::ThreadPoolArguments m_threadPoolArguments;
        std::vector<std::string> m_symbols;
        core::Configuration m_loggerConfiguration;
};


#endif