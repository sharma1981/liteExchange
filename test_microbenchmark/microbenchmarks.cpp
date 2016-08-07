#include <cstddef>
#include <string>

#include <utility/microbenchmark.hpp>
#include "order_book_benchmark.h"

using namespace std;

#define EXECUTION_NUMBER 1000
void executeBenchmark(const std::string& benchmarkName, size_t executionNumber, utility::BenchmarkFunction function);

int main()
{
    executeBenchmark(ORDER_BOOK_BENCHMARK_NAME, EXECUTION_NUMBER, orderBookMatchingBenchmark);
    return 0;
}

void executeBenchmark(const std::string& benchmarkName, size_t executionNumber, utility::BenchmarkFunction function)
{
    utility::Microbenchmark benchmarker(benchmarkName, function, executionNumber);
    benchmarker.execute();
    benchmarker.displayResults();
}