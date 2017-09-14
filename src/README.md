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

