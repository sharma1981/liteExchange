#ifndef _THREAD_POOL_OBSERVER_H_
#define _THREAD_POOL_OBSERVER_H_

#include <core/concurrency/thread_pool.h>
#include <core/design_patterns/observer.hpp>
#include <core/logger/logger.h>

namespace core
{

class ThreadPoolObserver : public core::Observer<core::ThreadPool>
{
    public:

        void onEvent(const std::string& eventMessage) override
        {
            LOG_INFO("Thread pool", eventMessage.c_str())
        }
};


} // namespace

#endif