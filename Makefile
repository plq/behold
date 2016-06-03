all: logger

logger: logger.cpp
	g++ logger.cpp -o logger -Wall -Wextra -std=c++11
