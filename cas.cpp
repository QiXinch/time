#include <iostream>
#include "utils.h"

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		std::cout << "Usage: ./cas number" << std::endl;
		return 1;
	}

	unsigned num = std::atoi(argv[1]);
	unsigned *j = new unsigned;

	set_affinity(12);

	Freq freq;
	for(unsigned i = 0; i < num; ++i)
	{
		__asm__ __volatile__("LABEL0: \n\t");
		__asm__ __volatile__(
			"lock cmpxchgl %1, (%0) \n\t"
			: 
			: "r"(j), "r"(1)
			: "eax");
		__asm__ __volatile__("LABEL1: \n\t");
	}
	freq.end();
	freq.info();

	std::cout << "cas time: " << (((freq.etime.tv_sec - freq.stime.tv_sec + (0.0 + freq.etime.tv_usec - freq.stime.tv_usec) / 1000000) * 1000000000) / num) << "ns" << std::endl;

	return -1;
}
