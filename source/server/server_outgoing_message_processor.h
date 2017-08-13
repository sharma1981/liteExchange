#ifndef _OUTGOING_MESSAGE_PROCESSOR_H_
#define _OUTGOING_MESSAGE_PROCESSOR_H_

#include <cassert>
#include <string>
#include <memory>
#include <boost/format.hpp>

#include <quickfix/Session.h>
#include <quickfix/fix42/ExecutionReport.h>

#include <order_matcher/central_order_book.h>
#include <server/quickfix_converter.h>
#include <core/concurrent/actor.h>
#include <core/logger/logger.h>

#include <server/server_constants.h>

using namespace core;
using namespace order_matcher;

class OutgoingMessageProcessor : public Actor
{
    public:

        OutgoingMessageProcessor() : Actor("OutgoingWorker"), m_messageQueue{nullptr}, m_execID{0}
        // We can`t have more than 16 characters in Linux for a pthread name ,that is why compacted the thread name...
        {
        }

        void setMessageQueue(OutgoingMessageQueue* queue)
        {
            assert( queue != nullptr );
            m_messageQueue = queue;
        }

        void* run() override
        {
            LOG_INFO("Outgoing message processor", "Outgoing message processor thread starting")

            // Let`s wait until message queue initialisation
            while (true)
            {
                if (isFinishing() == true)
                {
                    break;
                }

                if (m_messageQueue == nullptr)
                {
                    core::Thread::sleep(server_constants::SERVER_THREAD_SLEEP_DURATION);
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

                {//Creating scope for the smart ptr
                    std::unique_ptr<OutgoingMessage> message{ new OutgoingMessage };
                    if (m_messageQueue->dequeue(message.get()) == true)
                    {
                        const Order& order = message->getOrder();

                        LOG_INFO("Outgoing message processor", boost::str(boost::format("Processing %s for order : %s ") % message->toString() % order.toString()) )

                        FIX::TargetCompID targetCompID(order.getOwner());
                        FIX::SenderCompID senderCompID(order.getTarget());
                        auto status = convertToQuickFixOutgoingMessageType(message->getType());

                        FIX42::ExecutionReport fixOrder
                            (FIX::OrderID(order.getClientID()),
                            FIX::ExecID(genExecID()),
                            FIX::ExecTransType(FIX::ExecTransType_NEW),
                            FIX::ExecType(status),
                            FIX::OrdStatus(status),
                            FIX::Symbol(order.getSymbol()),
                            FIX::Side(order_matcher::convertOrderSideToQuickFix(order.getSide())),
                            FIX::LeavesQty(order.getOpenQuantity()),
                            FIX::CumQty(order.getExecutedQuantity()),
                            FIX::AvgPx(order.getAverageExecutedPrice()));

                        fixOrder.set(FIX::ClOrdID(order.getClientID()));
                        fixOrder.set(FIX::OrderQty(order.getQuantity()));

                        if (status == FIX::OrdStatus_FILLED ||
                            status == FIX::OrdStatus_PARTIALLY_FILLED)
                        {
                            fixOrder.set(FIX::LastShares(order.getLastExecutedQuantity()));
                            fixOrder.set(FIX::LastPx(order.getLastExecutedPrice()));
                        }

                        try
                        {
                            FIX::Session::sendToTarget(fixOrder, senderCompID, targetCompID);
                        }
                        catch (const FIX::SessionNotFound&)
                        {
                            // TO BE IMPLEMENTED
                        }

                    }
                    else
                    {
                        core::Thread::yield();
                    }
                }//Scope for the smart pointer


            }// while

            LOG_INFO("Outgoing message processor", "Outgoing message processor thread exiting")

            return nullptr;
        }

    private:

        OutgoingMessageQueue* m_messageQueue = nullptr;
        int m_execID;

        std::string genExecID()
        {
            ++m_execID;
            return std::to_string(m_execID);
        }
};

#endif