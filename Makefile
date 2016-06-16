
CXX=g++
CXXFLAGS := -g -Wall -Wextra -std=c++1y -I. $(HAVE_MSGPACK)

HAVE_MSGPACK := $(shell echo "\#include<msgpack.hpp>" \
        | $(CXX) -std=c++11 -E -x c++ - -o /dev/null 1>&2 2> /dev/null \
    && echo -DHAVE_MSGPACK)


all: behold test

behold: main.o behold.o
	$(CXX) main.o behold.o -o behold

behold.o: behold.cpp behold.h
	$(CXX) -c behold.cpp -o behold.o $(CXXFLAGS)

main.o: main.cpp behold.h
	$(CXX) -c main.cpp -o main.o $(CXXFLAGS)

clean:
	-rm -fv behold test *.o

test.o: behold.h test.cpp
	$(CXX) -c test.cpp -o test.o $(CXXFLAGS)

tf.o: tf.cpp tf.h
	$(CXX) -c tf.cpp -o tf.o $(CXXFLAGS)

test: behold.o test.o tf.o
	$(CXX) test.o behold.o tf.o -o test
