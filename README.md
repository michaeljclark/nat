# bigint

Simple arbitrary precision integer arithmetic header library implemented using C++ operator overloads.

## Project

The project includes the following files:

File           | Description
:---           | :---
bigint.h       | unsigned big integer class
bigint-test.cc | unit tests for the bigint class

## Testing

The code has been tested with the following compilers:

- GCC 6.2.1 (Debian Linux)
- Apple LLVM version 8.0.0 (clang-800.0.42.1)
- MSC 14.0 (Visual Studio Community 2015)


## Interface

The `bigint.h` header implements the following operators and methods:

- operator+=(const bigint &operand)
- operator-=(const bigint &operand)
- bigint& operator<<=(int shamt)
- bigint& operator>>=(int shamt)
- bigint& operator&=(const bigint &operand)
- bigint& operator|=(const bigint &operand)
- bigint operator+(const bigint &operand) const
- bigint operator-(const bigint &operand) const
- bigint operator<<(int shamt) const
- bigint operator>>(int shamt) const
- bigint operator&(const bigint &operand) const
- bigint operator|(const bigint &operand) const
- bool operator==(const bigint &operand) const
- bool operator<(const bigint &operand) const
- bool operator!=(const bigint &operand) const
- bool operator<=(const bigint &operand) const
- bool operator>(const bigint &operand) const
- bool operator>=(const bigint &operand) const
- bigint operator*(const bigint &operand)
- bigint operator/(const bigint &divisor)
- bigint operator%(const bigint &divisor)
- bigint pow(size_t operand)
- std::string to_string()
