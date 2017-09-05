AR          = ar
CC          = cc
CXX         = c++
FLEX        = /usr/local/bin/flex
BISON       = /usr/local/bin/bison
INCLUDES    = -Isrc -I/usr/local/include
DEBUG_FLAGS = -g
OPT_FLAGS   = -O3
WARN_FLAGS  = -Wall
CFLAGS      = $(DEBUG_FLAGS) $(OPT_FLAGS) $(WARN_FLAGS) $(INCLUDES)
CXXFLAGS    = $(DEBUG_FLAGS) $(OPT_FLAGS) $(WARN_FLAGS) $(INCLUDES) -std=c++11
LDFLAGS     = -L/usr/local/lib


# build targets

all: libs tests demos

libs: build/lib/libnat.a build/lib/libcalc.a

tests: build/bin/nat-tests build/bin/int-tests

demos: build/bin/calc-demo build/bin/nat-demo build/bin/gmp-demo build/bin/BigJavaDemo.class

clean: ; rm -fr build \
	demos/calc-parser.cc demos/calc-parser.hh demos/calc-scanner.cc \
	demos/stack.hh demos/position.hh demos/location.hh


# build rules


build/obj/calc-driver.o: demos/calc-driver.cc demos/calc-parser.hh
	@echo CC $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $<

demos/calc-parser.cc demos/calc-parser.hh: demos/calc-parser.yy
	$(BISON) --language=c++ -d -o demos/calc-parser.cc $<

demos/calc-scanner.cc: demos/calc-scanner.ll demos/calc-parser.hh
	$(FLEX) --c++ --stdinit -o $@ $<

build/obj/%.o: src/%.c
	@echo CC $@ ; mkdir -p $(@D) ; $(CC) $(CFLAGS) -c -o $@ $^

build/obj/%.o: src/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $^

build/obj/%.o: tests/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $^

build/obj/%.o: demos/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $^

build/obj/%.o: demos/%.c
	@echo CC $@ ; mkdir -p $(@D) ; $(CC) $(CFLAGS) -c -o $@ $^

build/obj/%.o: utils/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $^

build/lib/libcalc.a: build/obj/calc-driver.o build/obj/calc-parser.o build/obj/calc-scanner.o
	@echo AR $@ ; mkdir -p $(@D) ; $(AR) cr $@ $^

build/lib/libnat.a: build/obj/nat.o build/obj/int.o
	@echo AR $@ ; mkdir -p $(@D) ; $(AR) cr $@ $^

build/bin/nat-tests: build/obj/nat-tests.o build/lib/libnat.a
	@echo LD $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

build/bin/int-tests: build/obj/int-tests.o build/lib/libnat.a
	@echo LD $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

build/bin/nat-demo: build/obj/nat-demo.o build/lib/libnat.a
	@echo LD $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

build/bin/calc-demo: build/obj/calc-demo.o build/lib/libcalc.a build/lib/libnat.a
	@echo LD $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

build/bin/gmp-demo: build/obj/gmp-demo.o
	@echo LD $@ ; mkdir -p $(@D) ; $(CC) $(CFLAGS) $(LDFLAGS) -lgmp -o $@ $^

build/bin/BigJavaDemo.class: demos/BigJavaDemo.java
	@echo JAVAC $@ ; mkdir -p $(@D) ; javac -d $(@D) $^
