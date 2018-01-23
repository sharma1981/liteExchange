//Coming from QuickFix
#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#endif

#include <server/server_fix.h>

#include <algorithm>
#include <ctype.h>
#include <cstddef>

#include <quickfix/FileStore.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/Log.h>

#include <core/concurrency/thread_priority.h>

#include <order_matcher/order.h>
#include <order_matcher/security_manager.h>
#include <server/quickfix_converter.h>
using namespace order_matcher;

#include <core/file_utility.h>
#include <core/pretty_exception.h>
#include <core/logger/logger.h>

using namespace std;

ServerFix::ServerFix(const string& fixEngineConfigFile, const ServerConfiguration& serverConfiguration)
: m_fixEngineConfigFile( fixEngineConfigFile )
{
    if (!core::doesFileExist(m_fixEngineConfigFile))
    {
        THROW_PRETTY_RUNTIME_EXCEPTION(core::format("FIX configuration file %s does not exist", m_fixEngineConfigFile))
    }

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

    // Incoming message dispatcher initialisation
    m_dispatcher.setCentralOrderBook(&m_centralOrderBook);
    m_dispatcher.start();

    // Outgoing message processor initialisation
    m_outgoingMessageProcessor.setMessageQueue(m_centralOrderBook.getOutgoingMessageQueue(), symbolCount);
    m_outgoingMessageProcessor.start();

    //CLI
    m_commandLineInterface.setParentCentralOrderbook(&m_centralOrderBook);
}

void ServerFix::run()
{
    // FIX engine initialisation
    FIX::SessionSettings settings(m_fixEngineConfigFile);
    FIX::FileStoreFactory storeFactory(settings);
    FIX::ThreadedSocketAcceptor fixEngineAcceptor(*this, storeFactory, settings);
    // FIX engine start
    fixEngineAcceptor.start();

    LOG_INFO("FIX Engine", "Acceptor started")

    m_commandLineInterface.run();

    // FIX engine stop
    fixEngineAcceptor.stop(true);
    LOG_INFO("FIX Engine", "Acceptor stopped")
}

ServerFix::~ServerFix()
{
    m_outgoingMessageProcessor.shutdown();
    m_dispatcher.shutdown();
}

void ServerFix::onCreate(const FIX::SessionID& sessionID)
{
}

void ServerFix::fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
{
}

void ServerFix::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID)
{
}

void ServerFix::onLogon(const FIX::SessionID& sessionID)
{
    LOG_INFO("FIX Engine", "New logon , session ID : " + sessionID.toString())
}

void ServerFix::onLogout(const FIX::SessionID& sessionID)
{
    LOG_INFO("FIX Engine", "Logout , session ID : " + sessionID.toString())
}

void ServerFix::toApp(FIX::Message& message, const FIX::SessionID& sessionID) throw(FIX::DoNotSend)
{
    LOG_INFO("FIX Engine", "Sending fix message : " + message.toString() )
}

void ServerFix::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
    LOG_INFO("FIX Engine", "Receiving fix message : " + message.toString())
    crack(message, sessionID);
}

void ServerFix::onMessage(const FIX42::NewOrderSingle& message, const FIX::SessionID& sessionID)
{
    LOG_INFO("FIX Engine", "New order message received :" + message.toString())

    FIX::SenderCompID senderCompID;
    FIX::TargetCompID targetCompID;
    FIX::ClOrdID clOrdID;
    FIX::Symbol symbol;
    FIX::Side side;
    FIX::OrdType ordType;
    FIX::Price price;
    FIX::OrderQty orderQty;

    // For the time being , we are ignoring TimeInForce

    message.getHeader().get(senderCompID);
    message.getHeader().get(targetCompID);
    message.get(clOrdID);
    message.get(symbol);
    message.get(side);
    message.get(ordType);
    message.get(orderQty);

    OrderType newOrderType = convertOrderTypeFromQuickFix(ordType);
    OrderSide newOrderSide = convertOrderSideFromQuickFix(side);

    // Find security id of the symbol in our SecurityManager
    auto securityId = SecurityManager::getInstance()->getSecurityId(symbol);

    // Check if we support the symbol
    if (securityId == -1)
    {
        // Reject non supported order type or side
        Order rejectedOrder(clOrdID, securityId, senderCompID, targetCompID, OrderSide::BUY, OrderType::LIMIT, 0, 0);
        // Rather than pushing on to the dispatcher , directly push to the outgoing message processor via the central order book
        m_centralOrderBook.rejectOrder(rejectedOrder, "Non supported symbol");
    }

    if (newOrderType == OrderType::NON_SUPPORTED || newOrderSide == OrderSide::NON_SUPPORTED)
    {
        // Reject non supported order type or side
        Order rejectedOrder(clOrdID, securityId, senderCompID, targetCompID, OrderSide::BUY, OrderType::LIMIT, 0, 0);
        // Rather than pushing on to the dispatcher , directly push to the outgoing message processor via the central order book
        m_centralOrderBook.rejectOrder(rejectedOrder, "Non supported order type or order side");
    }

    message.get(price);

    Order order(clOrdID, securityId, senderCompID, targetCompID, newOrderSide, newOrderType, price, (long)orderQty);
    IncomingMessage incomingMessage(order, order_matcher::IncomingMessageType::NEW_ORDER);
    m_dispatcher.pushMessage(incomingMessage);
}

void ServerFix::onMessage(const FIX42::OrderCancelRequest& message, const FIX::SessionID&)
{
    LOG_INFO("FIX Engine", "Cancel order message received :" + message.toString())

    FIX::OrigClOrdID origClOrdID;
    FIX::Symbol symbol;
    FIX::Side side;
    FIX::SenderCompID senderCompID;
    FIX::ClOrdID clientID;

    message.getHeader().get(senderCompID);
    message.get(origClOrdID);
    message.get(clientID);
    message.get(symbol);
    message.get(side);

    auto securityId = SecurityManager::getInstance()->getSecurityId(symbol);

    Order order("", securityId, senderCompID, "", convertOrderSideFromQuickFix(side), order_matcher::OrderType::LIMIT, 0, 0);
    IncomingMessage incomingMessage(order, order_matcher::IncomingMessageType::CANCEL_ORDER, origClOrdID);
    m_dispatcher.pushMessage(incomingMessage);
}