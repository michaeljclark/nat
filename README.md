# nat

Simple arbitrary precision natural number and signed integer arithmetic library
with demo expression parser and compiler.

## Project

The project includes the following files:

File                   | Description
:---                   | :---
src/nat.h              | unsigned natural number header
src/nat.cc             | unsigned natural number impl
src/int.h              | integer with sign and magnitude header
src/int.cc             | integer with sign and magnitude impl
tests/nat-tests.cc     | unit tests for the Nat class
tests/int-tests.cc     | unit tests for the Int class
demos/nat-demo.cc      | simple arbitrary precision calculator


## Building

The following packages are required:

- bison 3.0.4
- flex 2.6.4
- libedit 20170329
- ncurses 6.0

### macOS dependencies

To install dependencies on macOS using brew:

`brew install bison flex libedit ncurses`

### Linux dependencies

To install dependencies on Debian or Ubuntu using apt:

`apt-get install bison flex libedit-dev libncurses-dev`

### Compiler support

The code has been tested with the following compilers:

- GCC 6.3.0 (Debian Linux)
- Apple LLVM version 8.1.0 (clang-802.0.42)


## Nat Demo

For an interactive nat-demo command line interface (use semicolon
as a continuation marker for multiple line expressions):

```
$ ./build/bin/nat-demo 
Nat> a = 2;
   | b = 128;
   | c = 2 ** b
 a = 2
 b = 128
 c = 340282366920938463463374607431768211456
Nat> 
```

To evaluate an expression with nat-demo:

```
$ ./build/bin/nat-demo --run examples/bswap.nat
 p = 5
 s = 83886080
```

To dump the parse tree with nat-demo:

```
$ ./build/bin/nat-demo --dump examples/bswap.nat
	(setvar 'p', (li 0x5))
	(setvar 's', (or (or (or (and (srl (var 'p'), (li 0x18)), (li 0xff)), (and (sll (var 'p'), (li 0x8)), (li 0xff0000))), (and (srl (var 'p'), (li 0x8)), (li 0xff00))), (and (sll (var 'p'), (li 0x18)), (li 0xff000000))))
```

To lower an expression to SSA form with nat-demo:

```
$ ./build/bin/nat-demo --lower examples/bswap.nat
	(setreg _0, (li 0x5))
	(setreg _1, (li 0x18))
	(setreg _2, (srl _0, _1))
	(setreg _3, (li 0xff))
	(setreg _4, (and _2, _3))
	(setreg _5, (li 0x8))
	(setreg _6, (sll _0, _5))
	(setreg _7, (li 0xff0000))
	(setreg _8, (and _6, _7))
	(setreg _9, (or _4, _8))
	(setreg _10, (li 0x8))
	(setreg _11, (srl _0, _10))
	(setreg _12, (li 0xff00))
	(setreg _13, (and _11, _12))
	(setreg _14, (or _9, _13))
	(setreg _15, (li 0x18))
	(setreg _16, (sll _0, _15))
	(setreg _17, (li 0xff000000))
	(setreg _18, (and _16, _17))
	(setreg _19, (or _14, _18))
```


## Nat Interface

struct Nat implements the following operators and methods:

- Nat(unsigned int n)
- Nat(std::string str, size_t radix = 0 /*autodetect*/)
- Nat& operator+=(const Nat &operand)
- Nat& operator-=(const Nat &operand)
- Nat& operator<<=(int shamt)
- Nat& operator>>=(int shamt)
- Nat& operator&=(const Nat &operand)
- Nat& operator|=(const Nat &operand)
- Nat operator+(const Nat &operand) const
- Nat operator-(const Nat &operand) const
- Nat operator<<(int shamt) const
- Nat operator>>(int shamt) const
- Nat operator&(const Nat &operand) const
- Nat operator|(const Nat &operand) const
- bool operator==(const Nat &operand) const
- bool operator<(const Nat &operand) const
- bool operator!=(const Nat &operand) const
- bool operator<=(const Nat &operand) const
- bool operator>(const Nat &operand) const
- bool operator>=(const Nat &operand) const
- bool operator!() const
- Nat operator*=(const Nat &operand) 
- Nat operator/=(const Nat &divisor)
- Nat operator%=(const Nat &divisor)
- Nat operator*(const Nat &operand) const
- Nat operator/(const Nat &divisor) const
- Nat operator%(const Nat &divisor) const
- Nat pow(size_t operand) const
- void from_string(std::string, size_t radix = 0 /*autodetect*/)
- std::string to_string(size_t radix = 10) const
