// when i use amd server, i find that its l3 cache shared on 3 cores, 
// so when binding the thread on 6,7,8 and when binding the thread on 6, 8, 9
// the result is differnent

#include <iostream>
#include <thread>
#include <sys/time.h>
#include <sys/mman.h>
#include "utils.h"

int main(int argc, char **argv)
{
    set_affinity(6);

    if(argc != 2)
    {
        std::cout << "Usage: ./sync num" << std::endl;
        return 1;
    }

    uint64_t num = static_cast<uint64_t>(std::atoll(argv[1]));

    uint64_t *sync = reinterpret_cast<uint64_t*>(mmap(nullptr, sizeof(uint64_t) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0));
    if(sync == nullptr)
    {
        std::cout << "malloc memory fail!" << std::endl;
        return 1;
    }

    *sync = num;
    uint64_t dummy = 0ul;

    auto fun0 = [&]() -> void {
        set_affinity(8);

        Freq freq;
        uint64_t dummy1;
        __asm__ __volatile__("movq (%1), %0" : "=r"(dummy1) : "r"(sync) :);

        //while(*sync == num);
        __asm__ __volatile__(
            "LOOP:\n\t"
            "   cmpq (%1), %0 \n\t"
            "   je LOOP \n\t"
            :
            : "r"(num), "r"(sync)
            :
        );

        uint64_t after_sync = rdtscp();

        freq.end();
        freq.info();

        std::cout << std::endl;
        auto cycles = after_sync - *sync;
        std::cout << "cycles = " << cycles << std::endl;
    };

    auto fun1 = [&]() -> void {
        set_affinity(9);

        uint64_t dummy1;
        __asm__ __volatile__("movq (%1), %0" : "=r"(dummy1) : "r"(sync) :);

        for(uint64_t i = 0; i < num; ++i)
            dummy += i;
        //__asm__ __volatile__("movq %0, 8(%1)" : : "i"(0ul), "r"(sync) : );

        *sync = rdtscp();
    };

    std::thread thread0(fun0);
    std::thread thread1(fun1);

    thread0.join();
    thread1.join();

    munmap(sync, sizeof(*sync) * 2);
    std::cout << "dummy = " << dummy << std::endl;
 
    return 0;
}
