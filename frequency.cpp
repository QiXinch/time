#include <iostream>
#include <sys/time.h>
#include "utils.h"

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cout << "Usage: ./frequency number" << std::endl;
        exit(1);
    }

    uint64_t num = static_cast<uint64_t>(std::atoll(argv[1]));

    set_affinity(12);

    timeval stime, etime;

    gettimeofday(&stime, nullptr);
    auto scycles = rdtscp();

    //-----------------------------------------------
    uint64_t dummy = 0;
    for(uint64_t i = 0; i < num; ++i)
        dummy += i;

    auto ecycles = rdtscp();

    auto cycles = ecycles - scycles;
    gettimeofday(&etime, nullptr);
    //-----------------------------------------------

    double time = (etime.tv_sec - stime.tv_sec) + (0.0 + etime.tv_usec - stime.tv_usec) / 1000000;

    std::cout << "dummy = " << dummy << std::endl;
    std::cout << "cycles: " << cycles << std::endl;
    std::cout << "time: "  << time << " s" << std::endl;
    std::cout << "frequency: " << ((cycles / (time * 1e9)) * 1e3) << " MHz" << std::endl;

    return 0;
}
