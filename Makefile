
HAVE_MSGPACK := $(shell echo "\#include<msgpack.hpp>" \
        | g++ -std=c++11 -E -x c++ - -o /dev/null 1>&2 2> /dev/null \
    && echo -DHAVE_MSGPACK)

CXXFLAGS := -g -Wall -Wextra -std=c++11 -I. $(HAVE_MSGPACK)

all: behold test

behold: main.o behold.o
	g++ main.o behold.o -o behold

behold.o: behold.cpp behold.h
	g++ -c behold.cpp -o behold.o $(CXXFLAGS)

main.o: main.cpp behold.h
	g++ -c main.cpp -o main.o $(CXXFLAGS)

clean:
	-rm -fv behold test *.o

test.o: behold.h test.cpp
	g++ -c test.cpp -o test.o $(CXXFLAGS)

test: behold.o test.o
	g++ test.o behold.o -o test
