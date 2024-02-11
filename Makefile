all : frequency sync

asm : frequency.s sync.s 

frequency : frequency.cpp
	g++ $^ -o $@ -O2

sync : sync.cpp
	g++ $^ -o $@ -O2 -pthread

frequency.s : frequency.cpp
	g++ -S -O2 $^

sync.s : sync.cpp
	g++ -S -O2 $^

clean : 
	rm -f frequency
	rm -f frequency.s
	rm -f sync
	rm -f sync.s
