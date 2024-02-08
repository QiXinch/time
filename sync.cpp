#include <iostream>
#include <thread>
#include <sys/time.h>
#include <sys/mman.h>
#include "utils.h"

int main(int argc, char **argv)
{
    set_affinity(8);

    if(argc != 2)
    {
        std::cout << "Usage: ./sync num" << std::endl;
        return 1;
    }

    uint64_t num = static_cast<uint64_t>(std::atoll(argv[1]));

    volatile uint64_t *sync = reinterpret_cast<volatile uint64_t*>(mmap(nullptr, sizeof(uint64_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0));
    if(sync == nullptr)
    {
        std::cout << "malloc memory fail!" << std::endl;
        return 1;
    }

    *sync = num;
    uint64_t dummy = 0ul;

    auto fun0 = [&]() -> void {
        set_affinity(9);
        std::cout << *sync;

        __asm__ __volatile__("fun0_s: \n\t" :::);
        while(*sync == num);
        uint64_t after_sync = rdtscp();
        __asm__ __volatile__("fun0_e: \n\t" :::);

        std::cout << std::endl;
        auto cycles = after_sync - *sync;
        std::cout << "cycles = " << cycles << std::endl;
    };

    auto fun1 = [&]() -> void {
        set_affinity(10);
        std::cout << *sync;
        for(uint64_t i = 0; i < num; ++i)
            dummy += i;

        __asm__ __volatile__("fun1_s: \n\t":::);
        *sync = rdtscp();
        __asm__ __volatile__("fun1_e: \n\t":::);
    };

    std::thread thread0(fun0);
    std::thread thread1(fun1);

    thread0.join();
    thread1.join();

    //munmap(sync, sizeof(*sync));
 
    return 0;
}
