
HAVE_MSGPACK := $(shell echo "\#include<msgpack.hpp>" \
        | g++ -std=c++11 -E -x c++ - -o /dev/null 1>&2 2> /dev/null \
    && echo -DHAVE_MSGPACK)

CXXFLAGS := -g -Wall -Wextra -std=c++11 -I. $(HAVE_MSGPACK)

all: logger

logger: main.o logger.o
	g++ main.o logger.o -o logger

logger.o: logger.cpp logger.h
	g++ -c logger.cpp -o logger.o $(CXXFLAGS)

main.o:
	g++ -c main.cpp -o main.o $(CXXFLAGS)

clean:
	-rm -fv logger *.o
