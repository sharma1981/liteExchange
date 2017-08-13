#ifndef _ORDER_QUEUE_H_
#define _ORDER_QUEUE_H_

#include "order.h"
#include "order_book.h"
#include "outgoing_message.h"

#include <vector>
#include <string>
#include <queue>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include <concurrent/thread.h>
#include <concurrent/queue_mpmc.hpp>
#include <concurrent/thread_pool.h>

#include <core/design_patterns/visitor.hpp>
#include <core/design_patterns/observer.hpp>

namespace order_matcher
{

using OutgoingMessageQueue = concurrent::QueueMPMC<OutgoingMessage>;

class CentralOrderBook : public boost::noncopyable, public core::Visitable<Order>, public core::Observable<CentralOrderBook>
{
    public:
        CentralOrderBook() : m_isMatchingMultithreaded{ false } {}

        ~CentralOrderBook()
        {
            m_orderBookThreadPool.shutdown();
        }

        void setSymbols(const std::vector<std::string>symbols);
        void accept(core::Visitor<Order>& v) override;
        void initialiseMultithreadedMatching(concurrent::ThreadPoolArguments& args);

        bool addOrder(const Order& order);
        void rejectOrder(const Order& order, const std::string& message);
        void cancelOrder(const Order& order, const std::string& origClientOrderID);

        OutgoingMessageQueue* getOutgoingMessageQueue() { return &m_outgoingMessages; }

        bool isMatchingMultithreaded() const { return m_isMatchingMultithreaded; }

    private:
        std::unordered_map<std::string, OrderBook> m_orderBookDictionary; // Symbol - OrderBook dictionary
        std::unordered_map<std::string, int> m_queueIDDictionary; // Symbol - Queue ID dictionary
        bool isSymbolSupported (const std::string& symbol) const ;

        bool m_isMatchingMultithreaded;
        std::vector<std::string> m_symbols;
        OutgoingMessageQueue m_outgoingMessages;
        concurrent::ThreadPool m_orderBookThreadPool;

        void* taskNewOrder(const Order& order);
        void* taskCancelOrder(const Order& order, const std::string& origClientOrderID);

        // Move ctor deletion
        CentralOrderBook(CentralOrderBook&& other) = delete;
        // Move assignment operator deletion
        CentralOrderBook& operator=(CentralOrderBook&& other) = delete;
};

} // namespace

#endif