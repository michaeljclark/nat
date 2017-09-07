AR          := $(shell which ar)
CC          := $(shell which cc)
CXX         := $(shell which c++)
FLEX        := $(shell which flex)
BISON       := $(shell which bison)
EDIT_CFLAGS := $(shell pkg-config ncurses --cflags) $(shell pkg-config libedit --cflags)
EDIT_LIBS   := $(shell pkg-config ncurses --libs) $(shell pkg-config libedit --libs)
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

demos: build/bin/nat-demo

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
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(EDIT_CFLAGS) -c -o $@ $^

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

build/bin/nat-demo: build/obj/nat-demo.o build/lib/libcalc.a build/lib/libnat.a
	@echo LD $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(EDIT_LIBS)
