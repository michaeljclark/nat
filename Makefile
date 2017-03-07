CXX = g++
CXXFLAGS = -O3 -Wall -std=c++11

all: bin bin/bigint-tests

clean: ; rm -fr bin

bin: ; mkdir bin

bin/bigint-tests: src/bigint-tests.cc src/bigint.h
	$(CXX) $(CXXFLAGS) -o $@ $<
