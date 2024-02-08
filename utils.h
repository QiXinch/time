#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <sys/time.h>
#include <sched.h>
#include <unistd.h>

static void set_affinity(int cpu)
{
    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    CPU_SET(cpu, &cpus);

    if(sched_setaffinity(getpid(), sizeof(cpus), &cpus) == -1)
    {
        std::cout << "set affinity on cpu " << cpu << " fail!" << std::endl; 
        exit(1);
    }
}

inline uint64_t rdtscp()
{
    uint32_t high, low;
    __asm__ __volatile__ (
        "rdtscp \n\t"
        : "=d"(high), "=a"(low) :: "ecx" 
    );

    return (static_cast<uint64_t>(high) << 32) | static_cast<uint64_t>(low);
}

class Freq
{
    private:
        timeval stime;
        timeval etime;
        uint64_t scycles;
        uint64_t ecycles;

    public:
        Freq()
        {
            gettimeofday(&stime, nullptr);
            scycles = rdtscp();
        }

        void begin()
        {
            gettimeofday(&stime, nullptr);
            scycles = rdtscp();
        }

        void end()
        {
            gettimeofday(&etime, nullptr);
            ecycles = rdtscp();
        }

        void info()
        {
            uint64_t cycles = ecycles - scycles;
            double time = etime.tv_sec - stime.tv_sec + (0.0 + etime.tv_usec - stime.tv_usec) / 1000000;

            std::cout << "############################" << std::endl;
            std::cout << "cycles: " << cycles << std::endl;
            std::cout << "time: " << time << "s" << std::endl;
            std::cout << "frequency: " << ((cycles / (time * 1e9)) * 1e3) << " MHz" << std::endl;
            std::cout << "############################" << std::endl;
        }
};

#endif
