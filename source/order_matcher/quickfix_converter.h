#ifndef _QUICK_FIX_CONVERTER_H_
#define _QUICK_FIX_CONVERTER_H_

#include <exception>

#include "outgoing_message.h"
#include "order.h"

#include <quickfix/Session.h>
#include <quickfix/Values.h>

namespace order_matcher
{

inline OrderType convertOrderTypeFromQuickFix(const FIX::OrdType& ordType)
{
    OrderType ret = OrderType::NON_SUPPORTED;
    switch ( ordType )
    {
        case FIX::OrdType_LIMIT:
            ret = OrderType::LIMIT;
    }
    return ret;
}

inline OrderSide convertOrderSideFromQuickFix(const FIX::Side& side)
{
    OrderSide ret = OrderSide::NON_SUPPORTED;
    switch ( side )
    {
        case FIX::Side_BUY:
            ret = OrderSide::BUY;
            break;

        case FIX::Side_SELL:
            ret = OrderSide::SELL;
            break;
    }
    return ret;
}

inline FIX::Side convertOrderSideToQuickFix(const OrderSide& side)
{
    switch (side)
    {
        case OrderSide::BUY:
            return FIX::Side_BUY;

        case OrderSide::SELL:
            return FIX::Side_SELL;

        default:
            throw std::invalid_argument("Unsupported Side, use buy or sell");
    }
}

inline char convertToQuickFixOutgoingMessageType(const OutgoingMessageType& type)
{
    switch (type)
    {
        case OutgoingMessageType::ACCEPTED:
            return FIX::OrdStatus_NEW;

        case OutgoingMessageType::CANCELED:
            return FIX::OrdStatus_CANCELED;

        case OutgoingMessageType::FILLED:
            return FIX::OrdStatus_FILLED;

        case OutgoingMessageType::PARTIALLY_FIELD:
            return FIX::OrdStatus_PARTIALLY_FILLED;

        case OutgoingMessageType::REJECTED:
            return FIX::OrdStatus_REJECTED;

        default:
            throw std::invalid_argument("Unsupported outgoing message type");
    }
}

} // namespace

#endif