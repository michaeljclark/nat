AR          := $(shell which ar)
CXX         := $(shell which c++)
FLEX        := $(shell which flex)
BISON       := $(shell which bison)
EDIT_CFLAGS := $(shell pkg-config ncurses --cflags) $(shell pkg-config libedit --cflags)
EDIT_LIBS   := $(shell pkg-config ncurses --libs) $(shell pkg-config libedit --libs)
INCLUDES    = -Isrc -I/usr/local/include
DEBUG_FLAGS = -g
OPT_FLAGS   = -O3
WARN_FLAGS  = -Wall
CXXFLAGS    = $(DEBUG_FLAGS) $(OPT_FLAGS) $(WARN_FLAGS) $(INCLUDES) -std=c++11
LDFLAGS     = -L/usr/local/lib -Lbuild/lib -lnat


NAT_OBJS    = \
			build/obj/nat.o

NATC_OBJS	= \
			build/obj/nat-compiler.o \
			build/obj/nat-parser.o \
			build/obj/nat-scanner.o \
			build/obj/nat-target.o \
			build/obj/nat-target-riscv.o

# build targets

all: libs tests demo

libs: build/lib/libnat.a build/lib/libnatc.a

tests: build/bin/nat-tests

demo: build/bin/nat-repl

clean: ; rm -fr build \
	demo/nat-parser.cc demo/nat-parser.hh demo/nat-scanner.cc \
	demo/stack.hh demo/position.hh demo/location.hh


# build rules

demo/nat-parser.cc demo/nat-parser.hh: demo/nat-parser.yy
	$(BISON) --language=c++ -d -o demo/nat-parser.cc $<

demo/nat-scanner.cc: demo/nat-scanner.ll demo/nat-parser.hh
	$(FLEX) --c++ --stdinit -o $@ $<

build/obj/nat-compiler.o: demo/nat-compiler.cc demo/nat-parser.hh
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $<

build/obj/%.o: src/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $<

build/obj/%.o: tests/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $<

build/obj/%.o: demo/%.cc
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(EDIT_CFLAGS) -c -o $@ $<

build/lib/libnat.a: $(NAT_OBJS)
	@echo AR $@ ; mkdir -p $(@D) ; $(AR) cr $@ $^

build/lib/libnatc.a: $(NATC_OBJS)
	@echo AR $@ ; mkdir -p $(@D) ; $(AR) cr $@ $^

build/bin/nat-tests: build/obj/nat-tests.o libs
	@echo LD $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

build/bin/nat-repl: build/obj/nat-repl.o libs
	@echo LD $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -o $@ $< -lnatc $(LDFLAGS) $(EDIT_LIBS)
