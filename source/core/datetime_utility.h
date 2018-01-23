#ifndef _DATE_TIME_UTILITY_H_
#define _DATE_TIME_UTILITY_H_

#include <string>
#include <chrono>
#include <sstream>
#include <ctime>
#include <cstddef>
#include <iomanip>
#include "string_utility.h"

#if __linux__
#include <sys/time.h>
#endif

namespace core
{

enum class DateTimeFormat
{
    NON_UTC_SECONDS,
    NON_UTC_MILLISECONDS,
    NON_UTC_MICROSECONDS,
    UTC_SECONDS,
    UTC_MILLISECONDS,
    UTC_MICROSECONDS
};

// Could use anonymous namespace or static keyword since C++11 removed deprecation
// Even though functions here don`t operate on static data
// preferred inline to avoid code bloat

inline std::string getCurrentDateTime(const std::string format, bool universalTime)
{
    std::stringstream ss;
    std::string formatWithoutSubSeconds = core::split(format, 'S')[0] + 'S';

#if defined( _MSC_VER ) || ( __GNUC__ > 4 )
    auto now = std::chrono::system_clock::now();
    auto inTimeT = std::chrono::system_clock::to_time_t(now);

    if (universalTime)
    {
        ss << std::put_time(std::gmtime(&inTimeT), formatWithoutSubSeconds.c_str());
    }
    else
    {
        ss << std::put_time(std::localtime(&inTimeT), formatWithoutSubSeconds.c_str());
    }

    if (core::contains(format, "%%03u") == true )
    {
        // Add milliseconds
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    }
    else if (core::contains(format, "%%06u") == true)
    {
        // Add microseconds
        auto ms = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;
        ss << '.' << std::setfill('0') << std::setw(6) << ms.count();
    }
#else
    // In C++11 std::put_time does this more easily, but in my tests
    // you need minimum GCC 5.1 , so using C library in this case
    time_t rawTime;
    struct tm * timeInfo;
    const std::size_t buffer_size = 32;
    char buffer[buffer_size];

    time(&rawTime);
    if (universalTime)
    {
        timeInfo = gmtime(&rawTime);
    }
    else
    {
        timeInfo = localtime(&rawTime);
    }

    strftime(buffer, buffer_size, formatWithoutSubSeconds.c_str(), timeInfo);
    ss << buffer;

    if (core::contains(format, "%%03u") == true )
    {
        // Add milliseconds
        timeval curTime;
        gettimeofday(&curTime, NULL);
        int milli = curTime.tv_usec / 1000;
        ss << '.' << std::setfill('0') << std::setw(3) << milli;
    }
    else if (core::contains(format, "%%06u") == true)
    {
        // Add microseconds
        timeval curTime;
        gettimeofday(&curTime, NULL);
        int micro = curTime.tv_usec;
        ss << '.' << std::setfill('0') << std::setw(6) << micro;
    }
#endif
    return ss.str();
}

inline std::string getCurrentDateTime(DateTimeFormat format, bool universalTime)
{
    std::string ret;
    switch (format)
    {
        case DateTimeFormat::NON_UTC_SECONDS:
            ret = getCurrentDateTime("%d-%m-%Y %H:%M:%S", universalTime);
            break;
        case DateTimeFormat::NON_UTC_MILLISECONDS:
            ret = getCurrentDateTime("%d-%m-%Y %H:%M:%S:%%03u", universalTime);
            break;
        case DateTimeFormat::NON_UTC_MICROSECONDS:
            ret = getCurrentDateTime("%d-%m-%Y %H:%M:%S:%%06u", universalTime);
            break;
        case DateTimeFormat::UTC_SECONDS:
            ret = getCurrentDateTime("%Y%m%d-%H:%M:%S", universalTime);
            break;
        case DateTimeFormat::UTC_MILLISECONDS:
            ret = getCurrentDateTime("%Y%m%d-%H:%M:%S.%%03u", universalTime);
            break;
        case DateTimeFormat::UTC_MICROSECONDS:
            ret = getCurrentDateTime("%Y%m%d-%H:%M:%S.%%06u", universalTime);
            break;
    }
    return ret;
}

}// namespace

#endif