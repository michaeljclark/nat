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
demos/nat-demo.cc      | simple arbitrary precision calculator

## Testing

The code has been tested with the following compilers:

- GCC 6.3.0 (Debian Linux)
- Apple LLVM version 8.1.0 (clang-802.0.42)
- MSC 15.0 (Visual Studio 2017)


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
