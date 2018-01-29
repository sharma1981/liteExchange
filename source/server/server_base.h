#ifndef _SERVER_BASE_H_
#define _SERVER_BASE_H_

#include <core/noncopyable.h>

#include <server/server_configuration.h>
#include <server/command_line_interface.h>
#include <server/server_outgoing_message_processor.h>

#include <order_matcher/central_order_book.h>
#include <order_matcher/central_order_book_observer.h>

class ServerBase : public core::NonCopyable
{
    public :
        explicit ServerBase(const ServerConfiguration& serverConfiguration)
        {
            // Central order book initialisation
            auto symbols = serverConfiguration.getSymbols();
            auto symbolCount = symbols.size();
            m_centralOrderBook.setSymbols(symbols);

            if (serverConfiguration.getMatchingMultithreadingMode() == true)
            {
                core::ThreadPoolArguments args = serverConfiguration.getThreadPoolArguments();
                args.m_threadNames = symbols;
                m_centralOrderBook.initialiseMultithreadedMatching(args);
            }

            m_centralOrderBook.initialiseOutgoingMessageQueues(symbolCount, serverConfiguration.getOutgoingMessageQueueSizePerThread());

            // Attach central order book observer to the central order book
            m_centralOrderBook.attach(m_centralOrderBookObserver);

            // Outgoing message processor initialisation
            m_outgoingMessageProcessor.setMessageQueue(m_centralOrderBook.getOutgoingMessageQueue(), symbolCount);
            m_outgoingMessageProcessor.start();

            //CLI
            m_commandLineInterface.setParentCentralOrderbook(&m_centralOrderBook);
        }

        virtual void run() = 0;

    protected:
        order_matcher::CentralOrderBook m_centralOrderBook;
        order_matcher::CentralOrderBookObserver m_centralOrderBookObserver;
        OutgoingMessageProcessor m_outgoingMessageProcessor;
        CommandLineInterface m_commandLineInterface;
};

#endif