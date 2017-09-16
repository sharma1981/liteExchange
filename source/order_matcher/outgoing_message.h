#ifndef _OUTGOING_MESSAGE_H_
#define _OUTGOING_MESSAGE_H_

#include <string>
#include "base_message.h"
#include "order.h"
#include <core/memory/heap_memory.h>
#include <core/pretty_exception.h>

namespace order_matcher
{

enum class OutgoingMessageType { ACCEPTED, FILLED, PARTIALLY_FIELD, CANCELED, REJECTED };

class OutgoingMessage : public BaseMessage, public core::Aligned<>
{
    public:

        OutgoingMessage()
        {
        }

        OutgoingMessage(Order order, OutgoingMessageType type, const std::string& message = "") : BaseMessage(order), m_type(type), m_message(message)
        {
        }

        bool hasMessage() const { return m_message.length() != 0 ; }

        OutgoingMessageType& getType() { return m_type; }

        std::string toString() const override
        {
            switch (m_type)
            {
                case order_matcher::OutgoingMessageType::ACCEPTED:
                    return "ACCEPTED";
                    break;
                case order_matcher::OutgoingMessageType::FILLED:
                    return "FILLED";
                    break;
                case order_matcher::OutgoingMessageType::PARTIALLY_FIELD:
                    return "PARTIALLY_FIELD";
                    break;
                case order_matcher::OutgoingMessageType::CANCELED:
                    return "CANCELED";
                    break;
                case order_matcher::OutgoingMessageType::REJECTED:
                    return "REJECTED";
                    break;
                default:
                    THROW_PRETTY_INVALID_ARG_EXCEPTION(std::string("Invalid outgoing message type"))
                    break;
            }
        }

    private:
        OutgoingMessageType m_type;
        std::string m_message;
};

} // namespace

#endif