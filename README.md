# nat

Simple arbitrary precision integer arithmetic header library implemented using C++ operator overloads.

## Project

The project includes the following files:

File        | Description
:---        | :---
nat.h       | natural number class
nat-test.cc | unit tests for the nat class

## Testing

The code has been tested with the following compilers:

- GCC 6.2.1 (Debian Linux)
- Apple LLVM version 8.0.0 (clang-800.0.42.1)
- MSC 14.0 (Visual Studio Community 2015)


## Interface

The `nat.h` header implements the following operators and methods:

- operator+=(const nat &operand)
- operator-=(const nat &operand)
- nat& operator<<=(int shamt)
- nat& operator>>=(int shamt)
- nat& operator&=(const nat &operand)
- nat& operator|=(const nat &operand)
- nat operator+(const nat &operand) const
- nat operator-(const nat &operand) const
- nat operator<<(int shamt) const
- nat operator>>(int shamt) const
- nat operator&(const nat &operand) const
- nat operator|(const nat &operand) const
- bool operator==(const nat &operand) const
- bool operator<(const nat &operand) const
- bool operator!=(const nat &operand) const
- bool operator<=(const nat &operand) const
- bool operator>(const nat &operand) const
- bool operator>=(const nat &operand) const
- nat operator*=(const nat &operand) 
- nat operator/=(const nat &divisor)
- nat operator%=(const nat &divisor)
- nat operator*(const nat &operand) const
- nat operator/(const nat &divisor) const
- nat operator%(const nat &divisor) const
- nat pow(size_t operand)
- std::string to_string()
