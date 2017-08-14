# nat

Simple arbitrary precision integer arithmetic header library implemented using C++ operator overloads.

## Project

The project includes the following files:

File        | Description
:---        | :---
nat.h       | natural number class
nat-test.cc | unit tests for the nat class
nat-demo.cc | multiply, pow, divide and to_string

## Testing

The code has been tested with the following compilers:

- GCC 6.2.1 (Debian Linux)
- Apple LLVM version 8.0.0 (clang-800.0.42.1)
- MSC 14.0 (Visual Studio Community 2015)


## Interface

The `nat.h` header implements the following operators and methods:

**Nat**

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
- std::string to_string() const

The `int.h` header implements the following operators and methods:

**Int**

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
