#include <iostream>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "utils.h"

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cout << "Usage: ./sync1 number" << std::endl;
        return 1;
    }

    uint64_t *sync = static_cast<uint64_t*>(mmap(nullptr, sizeof(uint64_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0));
    uint64_t num = static_cast<uint64_t>(std::atoll(argv[1]));

    set_affinity(11);

    *sync = num;
    uint64_t dummy0 = 0;
    uint64_t dummy1;
    uint64_t dummy2;

    pid_t child = fork();
    if(child == 0)
    {
        set_affinity(12);

        dummy1 = *sync;

        Freq freq;

        __asm__ __volatile__("child_s: \n\t":::);
        // FIXME when using -O2, modify sync1.s to avoid endless loop
        while(*sync == num);
        uint64_t sync1 = rdtscp();
        __asm__ __volatile__("child_e: \n\t":::);

        std::cout << "dummy1 = " << dummy1 << std::endl;
        std::cout << "sync cycles = " << (sync1 - *sync) << std::endl;

        freq.end();
        freq.info();
    }
    else
    {
        uint64_t dummy2 = *sync;

        __asm__ __volatile__("parent_s0: \n\t":::);
        for(uint64_t i = 0; i < num; ++i)
            dummy0 += i;
        __asm__ __volatile__("parent_e0: \n\t":::);

        __asm__ __volatile__("parent_s: \n\t":::);
        *sync = rdtscp();
        __asm__ __volatile__("parent_e: \n\t":::);

        int status;
        wait(&status);

        munmap(sync, sizeof(*sync));

        std::cout << "dummy0 = " << dummy0 << std::endl;
        std::cout << "dummy2 = " << dummy2 << std::endl;
        std::cout << "over" << std::endl;
    }

    return 0;
}
