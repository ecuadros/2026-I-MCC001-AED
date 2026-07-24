#include <iostream>
#include <vector>
#include <numeric>
#include <execution> // Paralelismo c++17
#include <chrono>
#include <random>

using TD = double;
// Auxiliar class to measure execution time
class Timer{
    std::chrono::high_resolution_clock::time_point start;
public:
    Timer() : start(std::chrono::high_resolution_clock::now()) {}
    void print(const std::string& name) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<TD, std::milli> elapsed = end - start;
        std::cout << "[Timer]" << name << " : " << elapsed.count() << " ms." << std::endl;
    }
};

int main () {
    const size_t dataSize = 50000000;
    std::cout << "[CPU] Memory assignment for " << dataSize << " elements..." << std::endl;
    std::vector<TD> v1(dataSize);
    std::vector<TD> v2(dataSize);

    for (size_t i = 0; i < dataSize; ++i) {
        v1[i] = (i % 100) * 0.5; // generando datos para los vectores
        v2[i] = (i % 50) * 0.1;
    }

    std::cout << "[CPU] Data ready.... starting Benchmarking " << std::endl;
    

    // using only one core
    TD sequentialResult;
    {
        std::cout << " Executing using only one core " << std::endl;
        Timer t;

        sequentialResult = std::transform_reduce(std::execution::seq, v1.begin(), v1.end(), v2.begin(), 0.0);

        t.print("Sequential time");
    }

    std::cout << std::endl;

    TD parallelResult;
    {
        std::cout << " Executing using multiple cores " << std::endl;
        Timer t;

        parallelResult = std::transform_reduce(std::execution::par_unseq, v1.begin(), v1.end(), v2.begin(), 0.0);

        t.print("Parallel time");
    
    }
    return 0;
}