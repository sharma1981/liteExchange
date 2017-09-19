#ifndef _OFFLINE_ORDER_ENTRY_
#define _OFFLINE_ORDER_ENTRY_

#include <string>
#include <vector>

#include <core/noncopyable.h>

#include <order_matcher/central_order_book.h>
#include <order_matcher/incoming_message.h>

#include <server/server_configuration.h>
#include <server/server_outgoing_message_processor.h>
#include <server/cli.h>

class OfflineOrderEntry
{
    public :
        explicit OfflineOrderEntry(const ServerConfiguration& serverConfiguration);
        void run();

    private :
        void loadOrders();
        void loadOrder(const std::string& line);

        std::string m_file;
        std::vector<order_matcher::IncomingMessage> m_orders;
        order_matcher::CentralOrderBook m_centralOrderBook;
        OutgoingMessageProcessor m_outgoingMessageProcessor;
        CommandLineInterface m_commandLineInterface;
};

#endif