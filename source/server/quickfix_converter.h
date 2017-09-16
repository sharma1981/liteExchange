#ifndef _QUICK_FIX_CONVERTER_H_
#define _QUICK_FIX_CONVERTER_H_

#include <core/pretty_exception.h>

#include <order_matcher/outgoing_message.h>
#include <order_matcher/order.h>

#include <quickfix/Session.h>
#include <quickfix/Values.h>

namespace
{

inline order_matcher::OrderType convertOrderTypeFromQuickFix(const FIX::OrdType& ordType)
{
    auto ret = order_matcher::OrderType::NON_SUPPORTED;
    switch (ordType)
    {
    case FIX::OrdType_LIMIT:
        ret = order_matcher::OrderType::LIMIT;
    }
    return ret;
}

inline order_matcher::OrderSide convertOrderSideFromQuickFix(const FIX::Side& side)
{
    auto ret = order_matcher::OrderSide::NON_SUPPORTED;
    switch (side)
    {
    case FIX::Side_BUY:
        ret = order_matcher::OrderSide::BUY;
        break;

    case FIX::Side_SELL:
        ret = order_matcher::OrderSide::SELL;
        break;
    }
    return ret;
}

inline FIX::Side convertOrderSideToQuickFix(const order_matcher::OrderSide& side)
{
    switch (side)
    {
    case order_matcher::OrderSide::BUY:
        return FIX::Side_BUY;

    case order_matcher::OrderSide::SELL:
        return FIX::Side_SELL;

    default:
        THROW_PRETTY_INVALID_ARG_EXCEPTION(std::string("Unsupported Side, use buy or sell"))
    }
}

inline char convertToQuickFixOutgoingMessageType(const order_matcher::OutgoingMessageType& type)
{
    switch (type)
    {
    case order_matcher::OutgoingMessageType::ACCEPTED:
        return FIX::OrdStatus_NEW;

    case order_matcher::OutgoingMessageType::CANCELED:
        return FIX::OrdStatus_CANCELED;

    case order_matcher::OutgoingMessageType::FILLED:
        return FIX::OrdStatus_FILLED;

    case order_matcher::OutgoingMessageType::PARTIALLY_FIELD:
        return FIX::OrdStatus_PARTIALLY_FILLED;

    case order_matcher::OutgoingMessageType::REJECTED:
        return FIX::OrdStatus_REJECTED;

    default:
        THROW_PRETTY_INVALID_ARG_EXCEPTION(std::string("Unsupported outgoing message type"))
    }
}

}

#endif