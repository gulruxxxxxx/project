CXX      ?= g++
CXXFLAGS  = -std=c++17 -Wall -Wextra -O2
SRC       = main.cpp helpers.cpp queue.cpp storage.cpp analytics.cpp dashboard.cpp menus.cpp demo.cpp

queueless: $(SRC) queueless.h
	$(CXX) $(CXXFLAGS) $(SRC) -o queueless

clean:
	rm -f queueless queueless.exe
