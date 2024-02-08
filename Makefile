all : frequency sync sync1

asm : frequency.s sync.s sync1.s

frequency : frequency.cpp
	g++ $^ -o $@ -O2

sync : sync.cpp
	g++ $^ -o $@ -O2 -pthread

sync1 : sync1.cpp
	g++ $^ -o $@ -O2

frequency.s : frequency.cpp
	g++ -S -O2 $^

sync.s : sync.cpp
	g++ -S -O2 $^

sync1.s : sync1.cpp
	g++ -S -O2 $^

clean : 
	rm -f frequency
	rm -f frequency.s
	rm -f sync
	rm -f sync.s
	rm -f sync1
	rm -f sync1.s
