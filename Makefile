all : frequency sync cas

asm : frequency.s sync.s  cas.s

frequency : frequency.cpp
	g++ $^ -o $@ -O2

cas : cas.cpp
	g++ $^ -o $@

sync : sync.cpp
	g++ $^ -o $@ -O2 -pthread

frequency.s : frequency.cpp
	g++ -S -O2 $^

sync.s : sync.cpp
	g++ -S -O2 $^

cas.s : cas.cpp
	g++ -S $^

clean : 
	rm -f frequency
	rm -f frequency.s
	rm -f sync
	rm -f sync.s
	rm -f cas
	rm -f cas.s
