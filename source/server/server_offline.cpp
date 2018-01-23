#include "server_offline.h"

#include <core/file_utility.h>
#include <core/string_utility.h>

#include <fix/fix_constants.h>
#include <fix/fix_message.h>

#include <order_matcher/security_manager.h>

#include <server/server_constants.h>

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

ServerOffline::ServerOffline(const ServerConfiguration& serverConfiguration)
{
    m_file = serverConfiguration.getOfflineOrderEntryFile();
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

    // Outgoing message processor initialisation
    m_outgoingMessageProcessor.setOfflineMode(server_constants::OFFLINE_ORDER_ENTRY_RESULTS_FILE);
    m_outgoingMessageProcessor.setMessageQueue(m_centralOrderBook.getOutgoingMessageQueue(), symbolCount);
    m_outgoingMessageProcessor.start();

    // Command line interface
    m_commandLineInterface.setParentCentralOrderbook(&m_centralOrderBook);
}

void ServerOffline::loadOrders()
{
    if (core::doesFileExist(m_file) == false)
    {
        return;
    }

    vector<fix::FixMessage> fixMessages;
    fix::FixMessage::loadFromFile(m_file, fixMessages);

    for(const auto& fixMessage : fixMessages)
    {
        IncomingMessageType type;
        auto messageType = fixMessage.getMessageType();

        // ORDER MESSAGE TYPE
        if (messageType == fix::FixConstants::FIX_MESSAGE_NEW_ORDER)
        {
            type = IncomingMessageType::NEW_ORDER;
        }
        else if (messageType == fix::FixConstants::FIX_MESSAGE_CANCEL_ORDER)
        {
            type = IncomingMessageType::CANCEL_ORDER;
        }
        else
        {
            // Skip
            continue;
        }

        //  SYMBOL
        string symbol;
        fixMessage.getTagValue(fix::FixConstants::FIX_TAG_SYMBOL, symbol);
        auto securityId = SecurityManager::getInstance()->getSecurityId(symbol);

        // ORDER SIDE
        string side;
        fixMessage.getTagValue(fix::FixConstants::FIX_TAG_ORDER_SIDE, side);
        int actualSide = std::stoi(side);
        OrderSide orderSide;

        if (actualSide == fix::FixConstants::FIX_ORDER_SIDE_BUY)
        {
            orderSide = OrderSide::BUY;
        }
        else if (actualSide == fix::FixConstants::FIX_ORDER_SIDE_SELL)
        {
            orderSide = OrderSide::SELL;
        }
        else
        {
            // Skip
            continue;
        }

        // PRICE
        string tempPrice;
        fixMessage.getTagValue(fix::FixConstants::FIX_TAG_ORDER_PRICE, tempPrice);
        double price = std::stod(tempPrice);

        // QUANTITY
        string tempQuantity;
        fixMessage.getTagValue(fix::FixConstants::FIX_TAG_ORDER_QUANTITY, tempQuantity);
        long quantity = std::stol(tempQuantity);

        Order order("", securityId, "", "", orderSide, OrderType::LIMIT, price, quantity);
        IncomingMessage message(order, type);
        m_orders.emplace_back(message);
    }

}

void ServerOffline::run()
{
    loadOrders();

    cout << "Loaded all orders into memory" << endl << endl;

    for (auto& order : m_orders)
    {
        // Currently not supporting cancel orders for offline mode
        if (order.getType() == IncomingMessageType::NEW_ORDER)
        {
            m_centralOrderBook.addOrder(order.getOrder());
        }
    }

    cout << "Submitted all orders to the central order book" << endl << endl;

    m_commandLineInterface.run();
}