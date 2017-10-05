#ifndef _INCOMING_MESSAGE_DISPATCHER_H_
#define _INCOMING_MESSAGE_DISPATCHER_H_

#include <cassert>
#include <string>
#include <memory>

#include <order_matcher/incoming_message.h>
#include <order_matcher/central_order_book.h>

#include <core/concurrency/thread.h>
#include <core/concurrency/thread_wait_strategy.h>
#include <core/concurrency/actor.h>
#include <core/concurrency/queue_mpsc.hpp>

#include <core/logger/logger.h>

#include <server/server_constants.h>

using namespace core;
using namespace order_matcher;


class IncomingMessageDispatcher : public Actor, public YieldWaitStrategy
{
    public:

        using IncomingMessageQueue = core::QueueMPSC<IncomingMessage>;
        using IncomingMessageQueueNode = core::QueueMPSC<IncomingMessage>::QueueMPSCNode;

        IncomingMessageDispatcher() : Actor("IncomingWorker"), m_centralOrderBook{nullptr}
        // We can`t have more than 16 characters in Linux for a pthread name ,that is why compacted the thread name...
        {
        }

        void setCentralOrderBook(order_matcher::CentralOrderBook* centralOrderBook)
        {
            assert(centralOrderBook != nullptr);
            m_centralOrderBook = centralOrderBook;
        }

        void pushMessage(const IncomingMessage& message)
        {
            m_queue.push(message);
        }

        void* run() override
        {
            LOG_INFO("Incoming message dispatcher", "Incoming message processor thread starting")
            // Let`s wait until central order book initialisation
            while (true)
            {
                if (isFinishing() == true)
                {
                    break;
                }

                if (m_centralOrderBook == nullptr)
                {
                    core::Thread::sleep(server_constants::SERVER_THREAD_SLEEP_DURATION_MICROSECONDS);
                }
                else
                {
                    break;
                }
            }

            while (true)
            {
                if (isFinishing() == true)
                {
                    break;
                }

                IncomingMessageQueueNode* messageLinkedList = nullptr;

                if ((messageLinkedList = m_queue.flush()) != nullptr)
                {
                    // Traverse linked list of incoming messages
                    while (messageLinkedList)
                    {
                        auto incomingMessage = messageLinkedList->m_data;

                        switch (incomingMessage.getType())
                        {
                            case IncomingMessageType::NEW_ORDER:
                                m_centralOrderBook->addOrder(incomingMessage.getOrder());
                            break;

                            case IncomingMessageType::CANCEL_ORDER:
                                m_centralOrderBook->cancelOrder(incomingMessage.getOrder(), incomingMessage.getOrigClientOrderID());
                            break;

                            default:
                            break;
                        }
                        std::unique_ptr< IncomingMessageQueueNode > temp{ messageLinkedList };
                        messageLinkedList = messageLinkedList->m_next;
                    }

                }
                else
                {
                    applyWaitStrategy(0);
                }
            }

            LOG_INFO("Incoming message dispatcher", "Incoming message processor thread exiting")
            return nullptr;
        }

    private :
        order_matcher::CentralOrderBook* m_centralOrderBook;
        core::QueueMPSC<IncomingMessage> m_queue;

};

#endif