
all:
	g++ -c -std=c++20 -fno-rtti vtable.cpp
	g++ -c -std=c++20 -fno-rtti main.cpp
	g++ vtable.o main.o
	./a.out

dump:
	g++ -E -std=c++20 main.cpp > dump