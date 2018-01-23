#include "central_order_book.h"
#include "security_manager.h"
#include <queue>
using namespace std;

#include <core/compiler/likely.h>
#include <core/stopwatch.h>
#include <core/string_utility.h>
using namespace core;

namespace order_matcher
{

void CentralOrderBook::accept(Visitor<Order>& v)
{
    for (auto& element : m_orderBookDictionary)
    {
        element.second.accept(v);
    }
}

void CentralOrderBook::setSymbols(const std::vector<std::string>& symbols)
{
    int queueID = -1;
    for (auto symbol : symbols)
    {
        auto currentSecurityId = SecurityManager::getInstance()->addSecurity(symbol);
        m_securityIds.push_back(currentSecurityId);
        m_queueIDDictionary.insert(make_pair(currentSecurityId, ++queueID));
    }
}

void CentralOrderBook::initialiseMultithreadedMatching(core::ThreadPoolArguments& args)
{
    m_orderBookThreadPool.attach(m_threadPoolObserver);
    notify(core::format("Initialising thread pool, work queue size per thread %d", args.m_workQueueSizePerThread) );

    m_isMatchingMultithreaded = true;

    // Thread names in args are also symbol names
    for (auto itr : m_securityIds)
    {
        auto securityId = itr;
        OrderBook currentOrderBook;
        m_orderBookDictionary.insert(make_pair(securityId, currentOrderBook));
    }

    m_orderBookThreadPool.initialise(args);
}

void CentralOrderBook::initialiseOutgoingMessageQueues(int numberOfThreads, int outgoingMessageProcessorQueueSize)
{
    for (auto i(0); i < numberOfThreads; i++)
    {
        unique_ptr<core::RingBufferSPSCLockFree<OutgoingMessage>> outgoingMessageQueue(new core::RingBufferSPSCLockFree<OutgoingMessage>(outgoingMessageProcessorQueueSize));
        m_outgoingMessages.push_back(std::move(outgoingMessageQueue));
    }
}

bool CentralOrderBook::addOrder(const Order& order)
{
    size_t securityId = order.getSecurityId();
    int queueID = m_queueIDDictionary[securityId];

    // SEND ACCEPTED MESSAGE TO THE CLIENT
    notify( core::format("New order accepted, client %s, client order ID %d ", order.getOwner(), order.getClientID()));
    OutgoingMessage message(order, OutgoingMessageType::ACCEPTED);
    m_outgoingMessages[queueID]->push(message);

    if (likely(m_isMatchingMultithreaded))
    {
        // MULTITHREADED MODE : SUBMIT NEW ORDER TASK TO THE THREAD POOL
        core::Task newOrderTask(&CentralOrderBook::taskNewOrder, this, order);
        m_orderBookThreadPool.submitTask(newOrderTask, queueID);
    }
    else
    {
        // SINGLE THREADED MODE : EXECUTE NEW ORDER SYNCHRONOUSLY
        taskNewOrder(order);
    }
    return true;
}

void* CentralOrderBook::taskNewOrder(const Order& order)
{
    StopWatch watch;
    watch.start();
    ////////////////////////////////////////////////////////////////
    size_t securityId = order.getSecurityId();
    int queueID = m_queueIDDictionary[securityId];

    m_orderBookDictionary[securityId].insert(order);

    queue<Order> processedOrders;
    m_orderBookDictionary[securityId].processMatching(processedOrders);

    auto processedOrderNum = processedOrders.size();

    // Append messages to outgoing queue
    // to let the clients know whether orders filled or partially filled
    while (processedOrders.size())
    {
        auto order = processedOrders.front();
        processedOrders.pop();

        // SEND FILLED OR PARTIALLY FILLED MESSAGE TO THE CLIENT
        OutgoingMessage message(order, order.isFilled() ? OutgoingMessageType::FILLED : OutgoingMessageType::PARTIALLY_FIELD);
        m_outgoingMessages[queueID]->push(message);
    }

    ////////////////////////////////////////////////////////////////
    watch.stop();
    notify(core::format("Order processing for symbol %s took %07ld microseconds , num of processed orders : %d", SecurityManager::getInstance()->getSecurityName(securityId), watch.getElapsedTimeMicroseconds(), processedOrderNum));
    return nullptr;
}

void CentralOrderBook::rejectOrder(const Order& order, const std::string& message)
{
    notify(core::format("Order being rejected, client %s, client ID %d", order.getOwner(), order.getClientID()));
    OutgoingMessage outgoingMessage(order, OutgoingMessageType::REJECTED, message);
    size_t securityId = order.getSecurityId();
    int queueID = m_queueIDDictionary[securityId];
    m_outgoingMessages[queueID]->push(outgoingMessage);
}

void CentralOrderBook::cancelOrder(const Order& order, const std::string& origClientOrderID)
{
    const string& owner = order.getOwner();
    size_t securityId = order.getSecurityId();
    // NOTIFY THE CLIENT
    notify(core::format("Order being canceled, client %s, client ID %d", owner, origClientOrderID));

    core::Task cancelOrderTask(&CentralOrderBook::taskCancelOrder, this, order, origClientOrderID);

    if (likely(m_isMatchingMultithreaded))
    {
        int queueID = m_queueIDDictionary[securityId];
        // MULTITHREADED MODE :SUBMIT CANCEL TASK TO THE THREADPOOL
        m_orderBookThreadPool.submitTask(cancelOrderTask, queueID);
    }
    else
    {
        // SINGLE THREADED MODE : CANCEL SYNCHRONOUSLY
        taskCancelOrder(order, origClientOrderID);
    }
}

void* CentralOrderBook::taskCancelOrder(const Order& order, const std::string& origClientOrderID)
{
    const string& owner = order.getOwner();
    size_t securityId = order.getSecurityId();
    int queueID = m_queueIDDictionary[securityId];
    const OrderSide& side = order.getSide();
    Order* orderBeingCanceled = nullptr;

    if (m_orderBookDictionary[securityId].find(&orderBeingCanceled, owner, origClientOrderID, side) == true)
    {
        orderBeingCanceled->cancel();
        OutgoingMessage outgoingMessage(*orderBeingCanceled, OutgoingMessageType::CANCELED, "Order canceled");
        m_outgoingMessages[queueID]->push(outgoingMessage);
        m_orderBookDictionary[securityId].erase(*orderBeingCanceled);
    }
    else
    {
        string message = core::format("Order to cancel could not be found, client %s, client ID %d", owner, origClientOrderID);
        notify(message);
        rejectOrder(order, message);
    }

    return nullptr;
}

}// namespace