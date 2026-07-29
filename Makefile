CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2

cache_sim: src/main.cpp src/Cache.cpp src/Cache.h
	$(CXX) $(CXXFLAGS) -o cache_sim src/main.cpp src/Cache.cpp

clean:
	rm -f cache_sim

.PHONY: clean