#ifndef _MICROBENCHMARK_HPP_
#define _MICROBENCHMARK_HPP_
/*
    Binds an execution to a CPU core ( core zero by default )
    Displays average min max times
    Colored output
    Reusable : reset method
*/

#include <cassert>
#include <cstddef>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <algorithm>

#include <boost/noncopyable.hpp>

#include "stopwatch.h"
#include "console_utility.h"

#include <concurrent/thread.h>

namespace utility
{

using BenchmarkFunction = std::function < void(void) > ;

class Microbenchmark : public boost::noncopyable
{
    public :
        Microbenchmark(const std::string& name, BenchmarkFunction function, std::size_t iterations)
        {
            reset(name, function, iterations);
        }

        ~Microbenchmark(){}

        void reset(const std::string& name, BenchmarkFunction function, std::size_t iterations)
        {
            m_Name = name;
            m_benchmarkFunction = function;
            m_iterations = iterations;
            assert(m_iterations > 0);
            m_results.reserve(m_iterations);
        }

        void execute()
        {
            concurrent::Thread::pinCallingThreadToCPUCore();

            std::size_t i{ 0 };
            for (; i < m_iterations; i++)
            {
                m_stopwatch.start();
                m_benchmarkFunction();
                m_stopwatch.stop();
                m_results.push_back(m_stopwatch.getElapsedTimeMilliseconds());
            }
        }

        void displayResults()
        {
            if (m_results.size() == 0)
            {
                utility::consoleOutputWithColor(ConsoleColor::FG_RED, "You have to execute it first");
            }
            else
            {
                std::stringstream stream;
                stream << "Benchmark name : " << m_Name << std::endl;
                stream << "Number of executions : " << m_iterations << std::endl;
                stream << "Total time : " << getTotalTime() << " milliseconds" << std::endl;
                stream << "Average time : " << getAverageTime() << " milliseconds"  << std::endl;
                stream << "Max time : " << getMaximumTime() << " milliseconds" << std::endl;
                stream << "Min time : " << getMinimumTime() << " milliseconds" << std::endl;

                utility::consoleOutputWithColor(ConsoleColor::FG_GREEN, stream.str().c_str());
            }
        }

        long long getMaximumTime() const
        {
            auto ret =  *(std::max_element(m_results.begin(), m_results.end()));
            return ret;
        }

        long long getMinimumTime() const
        {
            auto ret = *(std::min_element(m_results.begin(), m_results.end()));
            return ret;
        }

        long long getTotalTime() const
        {
            long long sum{ 0 };
            auto size = m_results.size();
            for (std::size_t i{ 0 }; i < size; i++)
            {
                sum += m_results[i];
            }
            return sum;
        }

        long long getAverageTime() const
        {
            auto size = m_results.size();
            auto sum = getTotalTime();
            return sum / (long long ) size;
        }

    private :
        std::string m_Name;
        BenchmarkFunction m_benchmarkFunction;
        std::size_t m_iterations;
        StopWatch m_stopwatch;
        std::vector<long long> m_results;

        // Move ctor deletion
        Microbenchmark(Microbenchmark&& other) = delete;
        // Move assignment operator deletion
        Microbenchmark& operator=(Microbenchmark&& other) = delete;
};

}// namespace

#endif