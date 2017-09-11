# nat

Simple arbitrary precision arithmetic library with simple expression
parser and compiler targetting a subset of the RISC-V ISA.

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
demo/nat-repl.cc       | simple compiler REPL
demo/nat-compiler.h    | simple compiler interface
demo/nat-compiler.cc   | simple compiler implementation
demo/nat-parser.yy     | simple compiler grammar specification
demo/nat-scanner.h     | simple compiler scanner interface
demo/nat-scanner.ll    | simple compiler lexical analyser specification

## Building

Packages required to build nat-repl:

Package Name | Version
:--          | --:
bison        | 3.0.4
flex         | 2.6.4
libedit      | 20170329
ncurses      | 6.0

Dependency installation for nat-repl:

Operating System | Installation method
:--              | :--
macOS            | `brew install bison flex libedit ncurses`
Debian or Ubuntu | `apt-get install bison flex libedit-dev libncurses-dev`

The code has been tested with the following compilers:

Compiler   | Version | Operating System
:--        | --:     | :--
GCC        | 6.3.0   | Debian Linux
Apple LLVM | 8.1.0   | macos 10.12.6


## Nat REPL

The Nat REPL is a simple expression evaluator.  For an interactive
command line interface execute nat-repl with no arguments. Use
semicolon as a continuation marker for multi-line expressions.

```
$ ./build/bin/nat-repl 
Nat> a = 2;
   | b = 128;
   | c = 2 << (b-1)
 a = 2 (0x2)
 b = 128 (0x80)
 c = 340282366920938463463374607431768211456 (0x100000000000000000000000000000000)
Nat> 
```


### Two’s complement subtraction using a Unicode syntax

```
$ cat examples/sub.nat
xlen ← 32
mask ← (1«xlen)-1
rs1 ← 9
rs2 ← 3
rd  ← (rs1 + ¬rs2 + 1) ∧ mask
```


### Population count example using C-like syntax

```
$ cat examples/popcount.nat
v1 = 0xf00f
v2 = ((v1 & 0xaaaaaaaa) >> 1)  + (v1 & 0x55555555)
v3 = ((v2 & 0xcccccccc) >> 2)  + (v2 & 0x33333333)
v4 = ((v3 & 0xf0f0f0f0) >> 4)  + (v3 & 0x0f0f0f0f)
v5 = ((v4 & 0xff00ff00) >> 8)  + (v4 & 0x00ff00ff)
v6 = ((v5 & 0xffff0000) >> 16) + (v5 & 0x0000ffff)
```


### Byte swap example using C-like syntax

```
$ cat examples/bswap.nat
p = 0x0a0b0c0d
s = ((p >> 24) & 0x000000ff) | ((p << 8) & 0x00ff0000) | ((p >> 8) & 0x0000ff00) | ((p << 24) & 0xff000000)
```


### Evaluating an expression from a file

```
$ ./build/bin/nat-repl --interp examples/bswap.nat
 p = 168496141 (0xa0b0c0d)
 s = 218893066 (0xd0c0b0a)
```


### Printing the abstract syntax tree for the expression

```
$ ./build/bin/nat-repl --tree examples/bswap.nat
	(setvar 'p', (const_int 0xa0b0c0d))
	(setvar 's', (or (or (or (and (srl (var 'p'), (const_int 0x18)), (const_int 0xff)), (and (sll (var 'p'), (const_int 0x8)), (const_int 0xff0000))), (and (srl (var 'p'), (const_int 0x8)), (const_int 0xff00))), (and (sll (var 'p'), (const_int 0x18)), (const_int 0xff000000))))
```


### Lowering the AST to an SSA form intermediate representation

```
$ ./build/bin/nat-repl --ssa examples/bswap.nat
	(setreg _0, (li 0xa0b0c0d))             v               
	(setreg _1, (srli _0, 0x18))            +v              
	(setreg _2, (li 0xff))                  ||v             
	(setreg _3, (and _1, _2))               |++v            
	(setreg _4, (slli _0, 0x8))             +  |v           
	(setreg _5, (li 0xff0000))              |  ||v          
	(setreg _6, (and _4, _5))               |  |++v         
	(setreg _7, (or _3, _6))                |  +  +v        
	(setreg _8, (srli _0, 0x8))             +      |v       
	(setreg _9, (li 0xff00))                |      ||v      
	(setreg _10, (and _8, _9))              |      |++v     
	(setreg _11, (or _7, _10))              |      +  +v    
	(setreg _12, (slli _0, 0x18))           +          |v   
	(setreg _13, (li 0xff000000))                      ||v  
	(setreg _14, (and _12, _13))                       |++v 
	(setreg _15, (or _11, _14))                        +  +v
```


### Performing register allocation on the SSA form IR

```
$ ./build/bin/nat-repl --regalloc examples/bswap.nat
	(setreg x1, (li 0xa0b0c0d))             v
	(setreg x2, (srli x1, 0x18))            +v
	(setreg x3, (li 0xff))                  ||v
	(setreg x4, (and x2, x3))               |++v
	(setreg x2, (slli x1, 0x8))             +v |
	(setreg x3, (li 0xff0000))              ||v|
	(setreg x5, (and x2, x3))               |++|v
	(setreg x2, (or x4, x5))                |v ++
	(setreg x4, (srli x1, 0x8))             +|v 
	(setreg x5, (li 0xff00))                |||v
	(setreg x3, (and x4, x5))               ||v++
	(setreg x4, (or x2, x3))                |++v 
	(setreg x2, (slli x1, 0x18))            +v |
	(setreg x1, (li 0xff000000))            v||
	(setreg x3, (and x2, x1))               ++v|
	(setreg x2, (or x4, x3))                 v++
```


### Outputting RISC-V like assembly language

```
$ ./build/bin/nat-repl --asm examples/bswap.nat
	li	x1, 0xa0b0c0d
	srli	x2, x1, 0x18
	li	x3, 0xff
	and	x4, x2, x3
	slli	x2, x1, 0x8
	li	x3, 0xff0000
	and	x5, x2, x3
	or	x2, x4, x5
	srli	x4, x1, 0x8
	li	x5, 0xff00
	and	x3, x4, x5
	or	x4, x2, x3
	slli	x2, x1, 0x18
	li	x1, 0xff000000
	and	x3, x2, x1
	or	x2, x4, x3
```


### Running the lowered machine code

```
./build/bin/nat-repl --run examples/bswap.nat
 x1 = 168496141 (0xa0b0c0d)
 x2 = 10 (0xa)
 x3 = 255 (0xff)
 x4 = 10 (0xa)
 x2 = 43135012096 (0xa0b0c0d00)
 x3 = 16711680 (0xff0000)
 x5 = 786432 (0xc0000)
 x2 = 786442 (0xc000a)
 x4 = 658188 (0xa0b0c)
 x5 = 65280 (0xff00)
 x3 = 2816 (0xb00)
 x4 = 789258 (0xc0b0a)
 x2 = 2826896152723456 (0xa0b0c0d000000)
 x1 = 4278190080 (0xff000000)
 x3 = 218103808 (0xd000000)
 x2 = 218893066 (0xd0c0b0a)
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
