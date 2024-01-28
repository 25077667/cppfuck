CXX ?= g++
CXXFLAGS ?= -std=c++20 -Ofast -Wall -Wextra -Wpedantic
OUT ?= main


.PHONY: all clean
$(OUT): main.cpp
	$(CXX) $(CXXFLAGS) -o main main.cpp

clean:
	rm -f $(OUT)