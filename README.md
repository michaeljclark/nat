# nat

Arbitrary precision arithmetic library with simple expression
compiler targetting a subset of the RISC-V ISA.

## Project

The project includes the following files:

File                   | Description
:---                   | :---
src/nat.h              | arbitrary precision unsigned natural number header
src/nat.cc             | arbitrary precision unsigned natural number implementation
src/int.h              | arbitrary precision integer with sign and magnitude header
src/int.cc             | arbitrary precision integer with sign and magnitude implementation
tests/nat-tests.cc     | unit tests for the Nat implementation
tests/int-tests.cc     | unit tests for the Int implementation
demo/nat-repl.cc       | simple compiler REPL
demo/nat-compiler.h    | simple compiler interface
demo/nat-compiler.cc   | simple compiler implementation
demo/nat-parser.yy     | simple compiler grammar specification
demo/nat-scanner.h     | simple compiler scanner interface
demo/nat-scanner.ll    | simple compiler lexical analyser specification
demo/nat-target[-*].h  | simple compiler target interface
demo/nat-target[-*].cc | simple compiler target implementation

## Building

### Packages required to build nat-repl

Package Name | Version
:--          | --:
bison        | 3.0.4
flex         | 2.6.4
libedit      | 20170329
ncurses      | 6.0

### Installation of build dependencies

Operating System | Installation method
:--              | :--
macOS            | `brew install bison flex libedit ncurses`
Debian or Ubuntu | `apt-get install bison flex libedit-dev libncurses-dev`

### Supported compilers and operating systems

Compiler   | Version | Operating System
:--        | --:     | :--
GCC        | 6.3.0   | Debian Linux
Apple LLVM | 8.1.0   | macos 10.12.6


## Nat REPL

The Nat REPL is a simple expression parser and evaluator.  For an
interactive command line interface execute nat-repl with no arguments.
Use semicolon as a continuation marker for multi-line expressions.

```
$ ./build/bin/nat-repl 
Nat> a = 1;
   | b = 128;
   | c = (a << b) + a
 a = 1 (0x1)
 b = 128 (0x80)
 c = 340282366920938463463374607431768211457 (0x100000000000000000000000000000001)
Nat> 
```


### Two’s complement subtraction example using Unicode syntax

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


### Printing the abstract syntax tree for an expression

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
	(setreg a0, (li 0xa0b0c0d))             v
	(setreg a1, (srli a0, 0x18))            +v
	(setreg a2, (li 0xff))                  ||v
	(setreg a3, (and a1, a2))               |++v
	(setreg a1, (slli a0, 0x8))             +v |
	(setreg a2, (li 0xff0000))              ||v|
	(setreg a4, (and a1, a2))               |++|v
	(setreg a1, (or a3, a4))                |v ++
	(setreg a3, (srli a0, 0x8))             +|v 
	(setreg a4, (li 0xff00))                |||v
	(setreg a2, (and a3, a4))               ||v++
	(setreg a3, (or a1, a2))                |++v 
	(setreg a1, (slli a0, 0x18))            +v |
	(setreg a0, (li 0xff000000))            v||
	(setreg a2, (and a1, a0))               ++v|
	(setreg a1, (or a3, a2))                 v++
```


### Outputting RISC-V assembly language

```
$ ./build/bin/nat-repl --asm examples/bswap.nat
	li	a0, 0xa0b0c0d
	srli	a1, a0, 0x18
	li	a2, 0xff
	and	a3, a1, a2
	slli	a1, a0, 0x8
	li	a2, 0xff0000
	and	a4, a1, a2
	or	a1, a3, a4
	srli	a3, a0, 0x8
	li	a4, 0xff00
	and	a2, a3, a4
	or	a3, a1, a2
	slli	a1, a0, 0x18
	li	a0, 0xff000000
	and	a2, a1, a0
	or	a1, a3, a2
```


### Running the lowered machine IR

```
./build/bin/nat-repl --run examples/bswap.nat
 a0 = 168496141 (0xa0b0c0d)
 a1 = 10 (0xa)
 a2 = 255 (0xff)
 a3 = 10 (0xa)
 a1 = 43135012096 (0xa0b0c0d00)
 a2 = 16711680 (0xff0000)
 a4 = 786432 (0xc0000)
 a1 = 786442 (0xc000a)
 a3 = 658188 (0xa0b0c)
 a4 = 65280 (0xff00)
 a2 = 2816 (0xb00)
 a3 = 789258 (0xc0b0a)
 a1 = 2826896152723456 (0xa0b0c0d000000)
 a0 = 4278190080 (0xff000000)
 a2 = 218103808 (0xd000000)
 a1 = 218893066 (0xd0c0b0a)
```
