#ifndef _SERVER_OFFLINE_H_
#define _SERVER_OFFLINE_H_

#include <string>
#include <vector>

#include <core/noncopyable.h>

#include <order_matcher/central_order_book.h>
#include <order_matcher/incoming_message.h>

#include <server/server_configuration.h>
#include <server/server_outgoing_message_processor.h>
#include <server/command_line_interface.h>

class ServerOffline
{
    public :
        explicit ServerOffline(const ServerConfiguration& serverConfiguration);
        void run();

    private :
        void loadOrders();

        std::string m_file;
        std::vector<order_matcher::IncomingMessage> m_orders;
        order_matcher::CentralOrderBook m_centralOrderBook;
        OutgoingMessageProcessor m_outgoingMessageProcessor;
        CommandLineInterface m_commandLineInterface;
};

#endif