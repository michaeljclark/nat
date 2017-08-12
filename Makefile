CXX = g++
CXXFLAGS = -O3 -Wall -std=c++11

all: bin bin/nat-tests

clean: ; rm -fr bin

bin: ; mkdir bin

bin/nat-tests: src/nat-tests.cc src/nat.h
	$(CXX) $(CXXFLAGS) -o $@ $<
