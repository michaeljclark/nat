# nat

Simple arbitrary precision natural number and signed integer arithmetic library.

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
demos/nat-demo.cc      | tests pow, multiply, divide and to_string
demos/gmp-demo.cc      | tests pow, multiply, divide and to_string
demos/BigJavaDemo.java | tests pow, multiply, divide and to_string

## Testing

The code has been tested with the following compilers:

- GCC 6.3.0 (Debian Linux)
- Apple LLVM version 8.1.0 (clang-802.0.42)
- MSC 14.0 (Visual Studio Community 2015)


## Interface

**Nat**

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

**Int**

struct Int implements the following operators and methods:

- Int(signed int n)
- bool operator==(const Int &operand) const
- bool operator<(const Int &operand) const
- bool operator!=(const Int &operand) const
- bool operator<=(const Int &operand) const
- bool operator>(const Int &operand) const
- bool operator>=(const Int &operand) const
- bool operator!() const
- Int& operator+=(const Int &operand)
- Int& operator-=(const Int &operand)
- Int& operator*=(const Int &operand) 
- Int& operator/=(const Int &divisor)
- Int& operator%=(const Int &divisor)
- Int operator+(const Int &operand) const
- Int operator-(const Int &operand) const
- Int operator*(const Int &operand) const
- Int operator/(const Int &divisor) const
- Int operator%(const Int &divisor) const
- std::string to_string() const
