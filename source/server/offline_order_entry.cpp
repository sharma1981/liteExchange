#include "offline_order_entry.h"

#include <core/file_utility.h>
#include <core/string_utility.h>

#include <order_matcher/security_manager.h>

#include <iostream>
#include <fstream>
using namespace std;

OfflineOrderEntry::OfflineOrderEntry(const ServerConfiguration& serverConfiguration)
{
    m_file = serverConfiguration.getOfflineOrderEntryFile();
    // Central order book initialisation
    m_centralOrderBook.setSymbols(serverConfiguration.getSymbols());

    if (serverConfiguration.getMatchingMultithreadingMode() == true)
    {
        core::ThreadPoolArguments args = serverConfiguration.getThreadPoolArguments();
        args.m_threadNames = serverConfiguration.getSymbols();
        m_centralOrderBook.initialiseMultithreadedMatching(args);
    }

    // Outgoing message processor initialisation
    m_outgoingMessageProcessor.setOfflineMode("offline_order_entry_results.txt");
    m_outgoingMessageProcessor.setMessageQueue(m_centralOrderBook.getOutgoingMessageQueue());
    m_outgoingMessageProcessor.start();

    //CLI
    m_commandLineInterface.setParentCentralOrderbook(&m_centralOrderBook);
}

void OfflineOrderEntry::loadOrders()
{
    if (core::doesFileExist(m_file) == false)
    {
        return;
    }

    ifstream file(m_file);

    file.seekg(0, std::ios::beg);
    string line;

    while (std::getline(file, line))
    {
        auto lineLength = line.length();

        if (line.c_str()[0] == '#' || lineLength == 0) // Skip comment lines and empty lines
        {
            continue;
        }

        loadOrder(line);
    }

    file.close();
}

void OfflineOrderEntry::loadOrder(const string& line)
{
    auto tokens = core::split(line, ',');

    IncomingMessageType type = (core::contains(tokens[0], "NEW_ORDER")) ? IncomingMessageType::NEW_ORDER : IncomingMessageType::CANCEL_ORDER;
    auto securityId = SecurityManager::getInstance()->getSecurityId(tokens[1]);
    OrderSide side = (core::contains(tokens[2], "SELL")) ? OrderSide::SELL : OrderSide::BUY;
    double price = std::stod(tokens[4]);
    long quantity = std::stol(tokens[5]);

    Order order("", securityId, "", "", side, OrderType::LIMIT, price, quantity);
    IncomingMessage message(order, type);
    m_orders.emplace_back(message);
}

void OfflineOrderEntry::run()
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