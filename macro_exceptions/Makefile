
PARAMS=-c -g -std=c++17

all:
	@g++ $(PARAMS) macro_exceptions.cpp
	@g++ $(PARAMS) test.cpp
	@g++ macro_exceptions.o test.o
	@./a.out

dump:
	@g++ -E test.cpp > dump.cpp