AR       = ar
CC       = cc
CXX      = c++
CFLAGS   = -g -O3 -Wall -I/usr/local/include
CXXFLAGS = -g -O3 -Wall -std=c++11 -Isrc
LDFLAGS  = -L/usr/local/lib


# build targets

all: libs tests exmaples

libs: build/lib/libnat.a

tests: build/bin/nat-tests build/bin/int-tests

exmaples: build/bin/nat-demo build/bin/gmp-demo build/bin/BigJavaDemo.class

clean: ; rm -fr build


# build rules

build/obj/%.o: src/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $^

build/obj/%.o: tests/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $^

build/obj/%.o: demos/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $^

build/obj/%.o: demos/%.c
	@echo CC $@ ; mkdir -p $(@D) ; $(CC) $(CFLAGS) -c -o $@ $^

build/lib/libnat.a: build/obj/nat.o build/obj/int.o
	@echo AR $@ ; mkdir -p $(@D) ; $(AR) cr $@ $^

build/bin/nat-tests: build/obj/nat-tests.o build/lib/libnat.a
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

build/bin/int-tests: build/obj/int-tests.o build/lib/libnat.a
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

build/bin/nat-demo: build/obj/nat-demo.o build/lib/libnat.a
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

build/bin/gmp-demo: build/obj/gmp-demo.o
	@echo CC $@ ; mkdir -p $(@D) ; $(CC) $(CFLAGS) $(LDFLAGS) -lgmp -o $@ $^

build/bin/BigJavaDemo.class: demos/BigJavaDemo.java
	@echo JAVAC $@ ; mkdir -p $(@D) ; javac -d $(@D) $^
