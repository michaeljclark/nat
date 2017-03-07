# bigint

Simple arbitrary precision integer arithmetic header library implemented using C++ operator overloads.

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
