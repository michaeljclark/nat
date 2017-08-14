CXX = g++
CXXFLAGS = -O3 -Wall -std=c++11

all: bin bin/nat-tests bin/int-tests bin/nat-demo

clean: ; rm -fr bin

bin: ; mkdir bin

bin/nat-tests: src/nat-tests.cc src/nat.h
	$(CXX) $(CXXFLAGS) -o $@ $<

bin/nat-demo: src/nat-demo.cc src/nat.h
	$(CXX) $(CXXFLAGS) -o $@ $<

bin/int-tests: src/int-tests.cc src/int.h src/nat.h
	$(CXX) $(CXXFLAGS) -o $@ $<
