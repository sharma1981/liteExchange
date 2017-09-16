#ifndef _DATE_TIME_UTILITY_H_
#define _DATE_TIME_UTILITY_H_

#include <string>
#include <chrono>
#include <sstream>
#include <ctime>
#include <cstddef>
#include <iomanip>
#include "string_utility.h"

namespace core
{

// Could use anonymous namespace or static keyword since C++11 removed deprecation
// Even though functions here don`t operate on static data
// preferred inline to avoid code bloat

// If format = %d-%m-%Y %H:%M:%S        , date time with just seconds
// If format = %d-%m-%Y %H:%M:"%S:%%03u , date time with milliseconds
// If format = %d-%m-%Y %H:%M:%S:%%06u    , date time with microseconds
inline std::string getCurrentDateTime(const char* format = "%d-%m-%Y %H:%M:%S:%%06u")
{
    std::stringstream ss;
#if defined( _MSC_VER ) || ( __GNUC__ > 4 )
    auto now = std::chrono::system_clock::now();
    auto inTimeT = std::chrono::system_clock::to_time_t(now);

    ss << std::put_time(std::localtime(&inTimeT), "%d-%m-%Y %H:%M:%S");

    if (core::contains(format, "%S:%%03u") == true )
    {
        // Add milliseconds
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    }
    else if (core::contains(format, "%S:%%06u") == true)
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
    timeInfo = localtime(&rawTime);

    strftime(buffer, buffer_size, "%d-%m-%Y %H:%M:%S", timeInfo);
    ss << buffer;

    if (core::contains(format, "%S:%%03u") == true )
    {
        // Add milliseconds
        timeval curTime;
        gettimeofday(&curTime, NULL);
        int milli = curTime.tv_usec / 1000;
        ss << '.' << std::setfill('0') << std::setw(3) << milli;
    }
    else if (core::contains(format, "%S:%%06u") == true)
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

}// namespace

#endif