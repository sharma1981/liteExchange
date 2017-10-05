#ifndef _OUTGOING_MESSAGE_PROCESSOR_H_
#define _OUTGOING_MESSAGE_PROCESSOR_H_

#include <cassert>
#include <string>
#include <memory>

#include <quickfix/Session.h>
#include <quickfix/fix42/ExecutionReport.h>

#include <order_matcher/central_order_book.h>
#include <order_matcher/security_manager.h>
#include <server/quickfix_converter.h>

#include <core/file_utility.h>
#include <core/compiler/likely.h>
#include <core/concurrency/thread_wait_strategy.h>
#include <core/concurrency/actor.h>
#include <core/logger/logger.h>
#include <core/datetime_utility.h>
#include <core/string_utility.h>

#include <server/server_constants.h>

using namespace core;
using namespace order_matcher;

class OutgoingMessageProcessor : public Actor, public YieldWaitStrategy
{
    public:

        OutgoingMessageProcessor() : Actor("OutgoingWorker"), m_messageQueue{ nullptr }, m_execID{ 0 }, m_offlineMode{false}
        // We can`t have more than 16 characters in Linux for a pthread name ,that is why compacted the thread name...
        {
        }

        void setOfflineMode(const std::string& offlineModeOutputFile)
        {
            m_offlineMode = true;
            m_offlineModeOutputFile = offlineModeOutputFile;
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

                {//Creating scope for the smart ptr
                    std::unique_ptr<OutgoingMessage> message{ new OutgoingMessage };
                    if (m_messageQueue->dequeue(message.get()) == true)
                    {
                        const Order& order = message->getOrder();

            LOG_INFO("Outgoing message processor", core::format("Processing %s for order : %s ", message->toString(), order.toString()))

                        FIX::TargetCompID targetCompID(order.getOwner());
                        FIX::SenderCompID senderCompID(order.getTarget());
                        auto status = convertToQuickFixOutgoingMessageType(message->getType());

                        FIX42::ExecutionReport fixOrder
                            (FIX::OrderID(order.getClientID()),
                            FIX::ExecID(genExecID()),
                            FIX::ExecTransType(FIX::ExecTransType_NEW),
                            FIX::ExecType(status),
                            FIX::OrdStatus(status),
                            FIX::Symbol(order_matcher::SecurityManager::getInstance()->getSecurityName(order.getSecurityId())),
                            FIX::Side(convertOrderSideToQuickFix(order.getSide())),
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

                        if (unlikely(m_offlineMode))
                        {
                            // OFFLINE ORDER ENTRY MODE
                            auto executionReportContent = fixOrder.toString();
                            core::appendTextToFile(m_offlineModeOutputFile, core::getCurrentDateTime() + '\n', true);
                            core::appendTextToFile(m_offlineModeOutputFile, executionReportContent + '\n', true);
                        }
                        else
                        {
                            // FIX ENGINE MODE
                            try
                            {
                                FIX::Session::sendToTarget(fixOrder, senderCompID, targetCompID);
                            }
                            catch (const FIX::SessionNotFound&)
                            {
                                // TO BE IMPLEMENTED
                            }
                        }
                    }
                    else
                    {
                        applyWaitStrategy(0);
                    }
                }//Scope for the smart pointer


            }// while

            LOG_INFO("Outgoing message processor", "Outgoing message processor thread exiting")

            return nullptr;
        }

    private:

        OutgoingMessageQueue* m_messageQueue = nullptr;
        int m_execID;
        bool m_offlineMode;
        std::string m_offlineModeOutputFile;

        std::string genExecID()
        {
            ++m_execID;
            return std::to_string(m_execID);
        }
};

#endif