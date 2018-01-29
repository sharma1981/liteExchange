#include "server_offline.h"

#include <core/file_utility.h>
#include <core/string_utility.h>
#include <core/logger/logger.h>

#include <fix/fix_constants.h>
#include <fix/fix_message.h>

#include <order_matcher/security_manager.h>

#include <server/server_constants.h>

#include <fstream>
#include <vector>
using namespace std;

ServerOffline::ServerOffline(const ServerConfiguration& serverConfiguration) :ServerBase(serverConfiguration)
{
    m_outgoingMessageProcessor.setOfflineMode(server_constants::OFFLINE_ORDER_ENTRY_RESULTS_FILE);
    m_file = serverConfiguration.getOfflineOrderEntryFile();
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

    LOG_INFO("Offline Server", "Loaded all orders into memory")

    for (auto& order : m_orders)
    {
        m_centralOrderBook.addOrder(order.getOrder());
    }

    LOG_INFO("Offline Server", "Submitted all orders to the central order book")

    m_commandLineInterface.run();
}