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

all: libs tests demo

libs: build/lib/libnat.a build/lib/libcalc.a

tests: build/bin/nat-tests build/bin/int-tests

demo: build/bin/nat-demo

clean: ; rm -fr build \
	demo/nat-parser.cc demo/nat-parser.hh demo/nat-scanner.cc \
	demo/stack.hh demo/position.hh demo/location.hh


# build rules


build/obj/nat-driver.o: demo/nat-driver.cc demo/nat-parser.hh
	@echo CC $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $<

demo/nat-parser.cc demo/nat-parser.hh: demo/nat-parser.yy
	$(BISON) --language=c++ -d -o demo/nat-parser.cc $<

demo/nat-scanner.cc: demo/nat-scanner.ll demo/nat-parser.hh
	$(FLEX) --c++ --stdinit -o $@ $<

build/obj/%.o: src/%.c
	@echo CC $@ ; mkdir -p $(@D) ; $(CC) $(CFLAGS) -c -o $@ $^

build/obj/%.o: src/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $^

build/obj/%.o: tests/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $^

build/obj/%.o: demo/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(EDIT_CFLAGS) -c -o $@ $^

build/obj/%.o: demo/%.c
	@echo CC $@ ; mkdir -p $(@D) ; $(CC) $(CFLAGS) -c -o $@ $^

build/obj/%.o: utils/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $^

build/lib/libcalc.a: build/obj/nat-driver.o build/obj/nat-parser.o build/obj/nat-scanner.o
	@echo AR $@ ; mkdir -p $(@D) ; $(AR) cr $@ $^

build/lib/libnat.a: build/obj/nat.o build/obj/int.o
	@echo AR $@ ; mkdir -p $(@D) ; $(AR) cr $@ $^

build/bin/nat-tests: build/obj/nat-tests.o build/lib/libnat.a
	@echo LD $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

build/bin/int-tests: build/obj/int-tests.o build/lib/libnat.a
	@echo LD $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

build/bin/nat-demo: build/obj/nat-demo.o build/lib/libcalc.a build/lib/libnat.a
	@echo LD $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(EDIT_LIBS)
