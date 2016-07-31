#include "stopwatch.h"
using namespace std::chrono;

namespace utility
{

void StopWatch::start ()
{
#if defined(__GNUC__) || _MSC_VER > 1800
    m_startTime = high_resolution_clock::now();
#else
    LARGE_INTEGER temp;
    QueryPerformanceFrequency(&temp);

    // For milliseconds , for seconds no division required
    m_frequency = static_cast<long long>(static_cast<double>(temp.QuadPart) / 1000.0);

    QueryPerformanceCounter(&temp);
    m_startTime = temp.QuadPart;
#endif
}

void StopWatch::stop ()
{
#if defined(__GNUC__) || _MSC_VER > 1800
    m_endTime = high_resolution_clock::now();
#else
    LARGE_INTEGER temp;
    QueryPerformanceCounter(&temp);
    m_endTime = temp.QuadPart;
#endif
}

long long StopWatch::getElapsedTimeMilliseconds ()
{
    long long ret{ 0 };
#if defined(__GNUC__) || _MSC_VER > 1800
    ret = duration_cast<std::chrono::milliseconds>(m_endTime - m_startTime).count();
#else
    ret = long long(m_endTime - m_startTime) / m_frequency;
#endif
    return ret;
}

}//namespace