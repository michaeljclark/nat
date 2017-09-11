# nat

Simple arbitrary precision arithmetic library with demo expression
parser and compiler.

## Project

The project includes the following files:

File                   | Description
:---                   | :---
src/nat.h              | unsigned natural number header
src/nat.cc             | unsigned natural number implementation
src/int.h              | integer with sign and magnitude header
src/int.cc             | integer with sign and magnitude implementation
tests/nat-tests.cc     | unit tests for the Nat class
tests/int-tests.cc     | unit tests for the Int class
demo/nat-demo.cc       | compiler CLI
demo/nat-driver.h      | compiler driver interface
demo/nat-driver.cc     | compiler driver implementation
demo/nat-parser.yy     | compiler grammar specifiction
demo/nat-scanner.h     | compiler scanner interface
demo/nat-scanner.ll    | compiler lexical analyser specifiction

## Building

Packages required to build nat-demo:

Package Name | Version
:--          | --:
bison        | 3.0.4
flex         | 2.6.4
libedit      | 20170329
ncurses      | 6.0

Dependency installation for nat-demo:

Operating System | Installation method
:--              | :--
macOS            | `brew install bison flex libedit ncurses`
Debian or Ubuntu | `apt-get install bison flex libedit-dev libncurses-dev`

The code has been tested with the following compilers:

Compiler   | Version | Operating System
:--        | --:     | :--
GCC        | 6.3.0   | Debian Linux
Apple LLVM | 8.1.0   | macos 10.12.6


## Nat Demo

For an interactive nat-demo command line interface (use semicolon
continuation marker for multi-line expressions):

```
$ ./build/bin/nat-demo 
Nat> a = 2;
   | b = 128;
   | c = 2 << (b-1)
 a = 2 (0x2)
 b = 128 (0x80)
 c = 340282366920938463463374607431768211456 (0x100000000000000000000000000000000)
Nat> 
```

View the source for a nat-demo example expression:

```
$ cat examples/bswap.nat
p = 127
s = ((p >> 24) & 0x000000ff) | ((p << 8) & 0x00ff0000) | ((p >> 8) & 0x0000ff00) | ((p << 24) & 0xff000000)
```

Evaluate an expression with nat-demo:

```
$ ./build/bin/nat-demo --run examples/bswap.nat
 p = 127 (0x7f)
 s = 2130706432 (0x7f000000)
```

Dump the parse tree with nat-demo:

```
$ ./build/bin/nat-demo --dump examples/bswap.nat
	(setvar 'p', (li 0x7f))
	(setvar 's', (or (or (or (and (srl (var 'p'), (li 0x18)), (li 0xff)), (and (sll (var 'p'), (li 0x8)), (li 0xff0000))), (and (srl (var 'p'), (li 0x8)), (li 0xff00))), (and (sll (var 'p'), (li 0x18)), (li 0xff000000))))
```

Lower the expression to SSA form with nat-demo:

```
$ ./build/bin/nat-demo --ssa examples/bswap.nat
	(setreg _0, (const_int 0x7f))           v               
	(setreg _1, (srli _0, 0x18))            +v              
	(setreg _2, (const_int 0xff))           ||v             
	(setreg _3, (and _1, _2))               |++v            
	(setreg _4, (slli _0, 0x8))             +  |v           
	(setreg _5, (const_int 0xff0000))       |  ||v          
	(setreg _6, (and _4, _5))               |  |++v         
	(setreg _7, (or _3, _6))                |  +  +v        
	(setreg _8, (srli _0, 0x8))             +      |v       
	(setreg _9, (const_int 0xff00))         |      ||v      
	(setreg _10, (and _8, _9))              |      |++v     
	(setreg _11, (or _7, _10))              |      +  +v    
	(setreg _12, (slli _0, 0x18))           +          |v   
	(setreg _13, (const_int 0xff000000))               ||v  
	(setreg _14, (and _12, _13))                       |++v 
	(setreg _15, (or _11, _14))                        +  +v
```

Lower the expression and allocate physical registers with nat-demo:


```
$ ./build/bin/nat-demo --regalloc examples/bswap.nat
	(setreg x1, (const_int 0x7f))           v
	(setreg x2, (srli x1, 0x18))            +v
	(setreg x3, (const_int 0xff))           ||v
	(setreg x4, (and x2, x3))               |++v
	(setreg x2, (slli x1, 0x8))             +v |
	(setreg x3, (const_int 0xff0000))       ||v|
	(setreg x5, (and x2, x3))               |++|v
	(setreg x2, (or x4, x5))                |v ++
	(setreg x4, (srli x1, 0x8))             +|v 
	(setreg x5, (const_int 0xff00))         |||v
	(setreg x3, (and x4, x5))               ||v++
	(setreg x4, (or x2, x3))                |++v 
	(setreg x2, (slli x1, 0x18))            +v |
	(setreg x1, (const_int 0xff000000))     v||
	(setreg x3, (and x2, x1))               ++v|
	(setreg x2, (or x4, x3))                 v++
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
