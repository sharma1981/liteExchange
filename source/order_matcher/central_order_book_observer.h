#ifndef _CENTRAL_ORDER_BOOK_OBSERVER_H_
#define _CENTRAL_ORDER_BOOK_OBSERVER_H_

#include "central_order_book.h"
#include <utility/design_patterns/observer.hpp>
#include <utility/logger/logger.h>

namespace order_matcher
{

class CentralOrderBookObserver : public utility::Observer<CentralOrderBook>
{
    public:

        void onEvent(const std::string& eventMessage) override
        {
            LOG_INFO("Central Order Book", eventMessage.c_str())
        }
};


} // namespace

#endif