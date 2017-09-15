/*
 * nat.cc
 *
 * unsigned natural number
 *
 * Copyright (C) 2017, Michael Clark <michaeljclark@mac.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <cassert>

#include "nat.h"

using limb_t = Nat::limb_t;
using limb2_t = Nat::limb2_t;

#if defined (__GNUC__)
#define clz __builtin_clz
#elif defined (_MSC_VER)
#include <intrin.h>
inline static unsigned long clz(unsigned int val)
{
	unsigned long count;
	return _BitScanReverse(&count, val) ? 31 - count : 0;
}
#else
#error clz not defined
#endif

const Nat::signedness Nat::_unsigned = { false };  /* unsigned */
const Nat::signedness Nat::_signed   = { true };   /* signed two's complement */


/*--------------.
| constructors. |
`--------------*/

/*! default constructor */
Nat::Nat(signedness s, unsigned bits)
	: limbs{0}, s(s), bits(bits) {}

/*! integral constructor */
Nat::Nat(const limb_t n, signedness s, unsigned bits)
	: limbs{n}, s(s), bits(bits)
{
	_contract();
}

/*! array constructor */
Nat::Nat(const std::initializer_list<limb_t> l, signedness s, unsigned bits)
	: limbs(l), s(s), bits(bits)
{
	_contract();
}

/*! string constructor */
Nat::Nat(std::string str, signedness s, unsigned bits)
	: limbs{0}, s(s), bits(bits)
{
	from_string(str.c_str(), str.size(), 0);
	this->s = s;
	this->bits = bits;
	_contract();
}


/*! string constructor with radix */
Nat::Nat(std::string str, size_t radix, signedness s, unsigned bits)
	: limbs{0}, s(s), bits(bits)
{
	from_string(str.c_str(), str.size(), radix);
	this->s = s;
	this->bits = bits;
	_contract();
}

/*! copy constructor  */
Nat::Nat(const Nat &operand)
	: limbs(operand.limbs), s(operand.s), bits(operand.bits)
{
	_contract();
}

/*! move constructor  */
Nat::Nat(const Nat&& operand) noexcept
	: limbs(std::move(operand.limbs)), s(operand.s), bits(operand.bits)
{
	_contract();
}


/*----------------------.
| assignment operators. |
`----------------------*/

/*! integral copy assignment operator */
Nat& Nat::operator=(const limb_t l)
{
	_resize(1);
	limbs[0] = l;
	return *this;
}

/*! Nat copy assignment operator */
Nat& Nat::operator=(const Nat &operand)
{
	limbs = operand.limbs;
	bits = operand.bits;
	s = operand.s;
	return *this;
}

/*! Nat move assignment operator */
Nat& Nat::operator=(Nat &&operand)
{
	limbs = std::move(operand.limbs);
	bits = operand.bits;
	s = operand.s;
	return *this;
}


/*------------------.
| internal methods. |
`------------------*/

/*! expand limbs to match operand */
void Nat::_expand(const Nat &operand)
{
	limbs.resize(std::min(max_limbs(), std::max(num_limbs(), operand.num_limbs())));
}

/*! contract zero big end limbs */
void Nat::_contract()
{
	while (bits > 0 && num_limbs() > max_limbs()) {
		limbs.pop_back();
	}
	while(num_limbs() > 1 && limbs.back() == 0) {
		limbs.pop_back();
	}
	if (bits > 0 && num_limbs() == max_limbs()) {
		limbs.back() &= limb_mask(num_limbs() - 1);
	}
}

/*! resize number of limbs */
void Nat::_resize(size_t n)
{
	limbs.resize(n);
}


/*-------------------------------.
| limb and bit accessor methods. |
`-------------------------------*/

/*! return number of limbs */
size_t Nat::num_limbs() const { return limbs.size(); }

/*! return maximum number of limbs */
size_t Nat::max_limbs() const { return ((bits - 1) >> limb_shift) + 1; }

/*! access word at limb offset */
limb_t Nat::limb_at(size_t n) const { return n < num_limbs() ? limbs[n] : 0; }

/*! limb_mask at limb offset */
limb_t Nat::limb_mask(size_t n) const
{
    if (bits == 0) return -1;
    if (n < (bits >> limb_shift)) return -1;
    if (n > (bits >> limb_shift)) return 0;
	else return ((1 << (bits & ((1ULL << limb_bits) - 1))) - 1);
}

/*! test bit at bit offset */
int Nat::test_bit(size_t n) const
{
	size_t word = n >> limb_shift;
	if (word < num_limbs()) return (limbs[word] >> (n & (limb_bits-1))) & 1;
	else return 0;
}

/*! set bit at bit offset */
void Nat::set_bit(size_t n)
{
	size_t word = n >> limb_shift;
	if (word >= num_limbs()) _resize(word + 1);
	limbs[word] |= (1ULL << (n & (limb_bits-1)));
}

/*! test sign */
bool Nat::sign_bit() const
{
	return s.is_signed && bits > 0 ? test_bit(bits - 1) : 0;
}


/*---------------------.
| mutating operations. |
`---------------------*/

/*! add with carry equals */
Nat& Nat::operator+=(const Nat &operand)
{
	_expand(operand);
	limb_t carry = 0;
	for (size_t i = 0; i < num_limbs(); i++) {
		limb_t old_val = limbs[i];
		limb_t new_val = old_val + operand.limb_at(i) + carry;
		limbs[i] = new_val;
		carry = new_val < old_val;
	}
	if (carry && num_limbs() < max_limbs()) {
		limbs.push_back(1);
	}
	return *this;
}

/*! subtract with borrow equals */
Nat& Nat::operator-=(const Nat &operand)
{
	_expand(operand);
	limb_t borrow = 0;
	for (size_t i = 0; i < num_limbs(); i++) {
		limb_t old_val = limbs[i];
		limb_t new_val = old_val - operand.limb_at(i) - borrow;
		limbs[i] = new_val;
		borrow = new_val > old_val;
	}
	_contract();
	return *this;
}

/*! left shift equals */
Nat& Nat::operator<<=(size_t shamt)
{
	size_t limb_shamt = shamt >> limb_shift;
	if (limb_shamt > 0) {
		limbs.insert(limbs.begin(), std::min(max_limbs(), limb_shamt), 0);
		shamt -= (limb_shamt << limb_shift);
	}
	_contract();
	if (!shamt) return *this;

	limb_t carry = 0;
	for (size_t j = 0; j < num_limbs(); j++) {
		limb_t old_val = limbs[j];
		limb_t new_val = (old_val << shamt) | carry;
		limbs[j] = new_val;
		carry = old_val >> (limb_bits - shamt);
	}
	if (carry && num_limbs() < max_limbs()) {
		limbs.push_back(carry);
	}
	_contract();
	return *this;
}

/*! right shift equals */
Nat& Nat::operator>>=(size_t shamt)
{
	size_t limb_shamt = shamt >> limb_shift;
	if (limb_shamt > 0) {
		limbs.erase(limbs.begin(), limbs.begin() + std::min(num_limbs(), limb_shamt));
		shamt -= (limb_shamt << limb_shift);
	}
	if (num_limbs() == 0) {
		*this = 0;
	}
	if (!shamt) return *this;

	limb_t carry = -(s.is_signed && sign_bit()) << (shamt-1);
	for (size_t j = num_limbs(); j > 0; j--) {
		limb_t old_val = limbs[j - 1];
		limb_t new_val = (old_val >> shamt) | carry;
		limbs[j - 1] = new_val;
		carry = old_val << (limb_bits - shamt);
	}
	_contract();
	return *this;
}

/*! bitwise and equals */
Nat& Nat::operator&=(const Nat &operand)
{
	_expand(operand);
	for (size_t i = 0; i < num_limbs(); i++) {
		limbs[i] = operand.limb_at(i) & limbs[i];
	}
	_contract();
	return *this;
}

/*! bitwise or equals */
Nat& Nat::operator|=(const Nat &operand)
{
	_expand(operand);
	for (size_t i = 0; i < num_limbs(); i++) {
		limbs[i] = operand.limb_at(i) | limbs[i];
	}
	_contract();
	return *this;
}

/*! bitwise xor equals */
Nat& Nat::operator^=(const Nat &operand)
{
	_expand(operand);
	for (size_t i = 0; i < num_limbs(); i++) {
		limbs[i] = operand.limb_at(i) ^ limbs[i];
	}
	_contract();
	return *this;
}


/*------------------.
| const operations. |
`------------------*/

/* const operations copy and use the mutating operations */

/*! add with carry */
Nat Nat::operator+(const Nat &operand) const
{
	Nat result(*this);
	return result += operand;
}

/*! subtract with borrow */
Nat Nat::operator-(const Nat &operand) const
{
	Nat result(*this);
	return result -= operand;
}

/*! left shift */
Nat Nat::operator<<(size_t shamt) const
{
	Nat result(*this);
	return result <<= shamt;
}

/*! right shift */
Nat Nat::operator>>(size_t shamt) const
{
	Nat result(*this);
	return result >>= shamt;
}

/*! bitwise and */
Nat Nat::operator&(const Nat &operand) const
{
	Nat result(*this);
	return result &= operand;
}

/*! bitwise or */
Nat Nat::operator|(const Nat &operand) const
{
	Nat result(*this);
	return result |= operand;
}

/*! bitwise xor */
Nat Nat::operator^(const Nat &operand) const
{
	Nat result(*this);
	return result ^= operand;
}

/*! bitwise not */
Nat Nat::operator~() const
{
	Nat result(*this);
	for (auto &n : result.limbs) {
		n = ~n;
	}
	return result;
}

/*! negate */
Nat Nat::operator-() const
{
	Nat result(*this);
	if (bits == 0) {
		return result;
	}
	size_t n = std::max(max_limbs(), num_limbs());
	result._resize(n);
	for (size_t i = 0; i < result.num_limbs(); i++) {
		result.limbs[i] = ~limb_at(i);
	}
	result += 1;
	result._contract();
	return result;
}


/*----------------------.
| comparison operators. |
`----------------------*/

/* comparison are defined in terms of "equals" and "less than" */

/*! equals */
bool Nat::operator==(const Nat &operand) const
{
	if (num_limbs() != operand.num_limbs()) return false;
	for (size_t i = 0; i < num_limbs(); i++) {
		if (limbs[i] != operand.limbs[i]) return false;
	}
	return true;
}

/*! less than */
bool Nat::operator<(const Nat &operand) const
{
	/* handle signed comparison if both operands are signed */
	if (bits > 0 && s.is_signed && operand.s.is_signed) {
		bool sign = sign_bit();
		if (sign ^ operand.sign_bit()) {
			return sign;
		} else if (sign) {
			return operand < *this;
		}
	}

	/* unsigned comparison */
	if (num_limbs() > operand.num_limbs()) return false;
	else if (num_limbs() < operand.num_limbs()) return true;
	for (ptrdiff_t i = num_limbs()-1; i >= 0; i--) {
		if (limbs[i] > operand.limbs[i]) return false;
		else if (limbs[i] < operand.limbs[i]) return true;
	}
	return false;
}

/* axiomatically define other comparisons in terms of "equals" and "less than" */

/*! not equals */
bool Nat::operator!=(const Nat &operand) const { return !(*this == operand); }

/*! less than or equal*/
bool Nat::operator<=(const Nat &operand) const { return *this < operand || *this == operand; }

/*! greater than */
bool Nat::operator>(const Nat &operand) const { return !(*this <= operand); }

/*! less than or equal*/
bool Nat::operator>=(const Nat &operand) const { return !(*this < operand) || *this == operand; }

/*! not */
bool Nat::operator!() const { return *this == 0; }


/*--------------------.
| multply and divide. |
`--------------------*/

/* These routines are derived from Hacker's Delight */

/*! base 2^limb_bits multiply */
void Nat::mult(const Nat &multiplicand, const Nat multiplier, Nat &result)
{
	size_t m = multiplicand.num_limbs(), n = multiplier.num_limbs();
	size_t k = std::min(multiplicand.max_limbs(), m + n);
	result._resize(k);
	limb_t carry = 0;
	limb2_t mj = multiplier.limbs[0];
	for (size_t i = 0; i < m && i < k; i++) {
		limb2_t t = limb2_t(multiplicand.limbs[i]) * mj + carry;
		result.limbs[i] = limb_t(t);
		carry = t >> limb_bits;
	}
	if (m < k) {
		result.limbs[m] = carry;
	}
	for (size_t j = 1; j < n; j++) {
		carry = 0;
		mj = multiplier.limbs[j];
		for (size_t i = 0; i < m && i + j < k; i++) {
			limb2_t t = limb2_t(multiplicand.limbs[i]) * mj + limb2_t(result.limbs[i + j]) + carry;
			result.limbs[i + j] = limb_t(t);
			carry = t >> limb_bits;
		}
		if (j + m < k) {
			result.limbs[j + m] = carry;
		}
	}
	result._contract();
}

/*! base 2^limb_bits division */
void Nat::divrem(const Nat &dividend, const Nat &divisor, Nat &quotient, Nat &remainder)
{
	quotient = 0;
	remainder = 0;
	ptrdiff_t m = dividend.num_limbs(), n = divisor.num_limbs();
	quotient._resize(std::max(m - n + 1, ptrdiff_t(1)));
	remainder._resize(n);
	limb_t *q = quotient.limbs.data(), *r = remainder.limbs.data();
	const limb_t *u = dividend.limbs.data(), *v = divisor.limbs.data();

	const limb2_t b = (1ULL << limb_bits); // Number base
	limb_t *un, *vn;                       // Normalized form of u, v.
	limb2_t qhat;                          // Estimated quotient digit.
	limb2_t rhat;                          // A remainder.

	if (m < n || n <= 0 || v[n-1] == 0) {
		quotient = 0;
		remainder = dividend;
		return;
	}

	// Single digit divisor
	if (n == 1) {
		limb2_t k = 0;
		for (ptrdiff_t j = m - 1; j >= 0; j--) {
			q[j] = limb_t((k*b + u[j]) / v[0]);
			k = (k*b + u[j]) - q[j]*v[0];
		}
		r[0] = limb_t(k);
		quotient._contract();
		remainder._contract();
		return;
	}

	// Normalize by shifting v left just enough so that
	// its high-order bit is on, and shift u left the
	// same amount. We may have to append a high-order
	// digit on the dividend; we do that unconditionally.

	int s = clz(v[n-1]); // 0 <= s <= limb_bits.
	vn = (limb_t *)alloca(sizeof(limb_t) * n);
	for (ptrdiff_t i = n - 1; i > 0; i--) {
		vn[i] = (v[i] << s) | (v[i-1] >> (limb_bits-s));
	}
	vn[0] = v[0] << s;

	un = (limb_t *)alloca(sizeof(limb_t) * (m + 1));
	un[m] = u[m-1] >> (limb_bits-s);
	for (ptrdiff_t i = m - 1; i > 0; i--) {
		un[i] = (u[i] << s) | (u[i-1] >> (limb_bits-s));
	}
	un[0] = u[0] << s;
	for (ptrdiff_t j = m - n; j >= 0; j--) { // Main loop.
		// Compute estimate qhat of q[j].
		qhat = (un[j+n]*b + un[j+n-1]) / vn[n-1];
		rhat = (un[j+n]*b + un[j+n-1]) - qhat * vn[n-1];
	again:
		if (qhat >= b || qhat*vn[n-2] > b*rhat + un[j+n-2]) {
			qhat = qhat - 1;
			rhat = rhat + vn[n-1];
			if (rhat < b) goto again;
		}
		// Multiply and subtract.
		limb2_t k = 0;
		slimb2_t t = 0;
		for (ptrdiff_t i = 0; i < n; i++) {
			unsigned long long p = qhat*vn[i];
			t = un[i+j] - k - (p & ((1ULL<<limb_bits)-1));
			un[i+j] = limb_t(t);
			k = (p >> limb_bits) - (t >> limb_bits);
		}
		t = un[j+n] - k;
		un[j+n] = limb_t(t);

		q[j] = limb_t(qhat); // Store quotient digit.
		if (t < 0) {         // If we subtracted too
			q[j] = q[j] - 1; // much, add back.
			k = 0;
			for (ptrdiff_t i = 0; i < n; i++) {
				t = un[i+j] + vn[i] + k;
				un[i+j] = limb_t(t);
				k = t >> limb_bits;
			}
			un[j+n] = limb_t(un[j+n] + k);
		}
	}

	// normalize remainder
	for (ptrdiff_t i = 0; i < n; i++) {
		r[i] = (un[i] >> s) | (un[i + 1] << (limb_bits - s));
	}

	quotient._contract();
	remainder._contract();
}

/*! multiply */
Nat Nat::operator*(const Nat &operand) const
{
	Nat result(0, s, bits);
	mult(*this, operand, result);
	result._contract();
	return result;
}

/*! division quotient */
Nat Nat::operator/(const Nat &divisor) const
{
	Nat quotient(0, s, bits), remainder(0, s, bits);
	divrem(*this, divisor, quotient, remainder);
	return quotient;
}

/*! division remainder */
Nat Nat::operator%(const Nat &divisor) const
{
	Nat quotient(0), remainder(0);
	divrem(*this, divisor, quotient, remainder);
	return remainder;
}

/*! multiply equals */
Nat& Nat::operator*=(const Nat &operand)
{
	Nat result = *this * operand;
	*this = std::move(result);
	return *this;
}

/*! divide equals */
Nat& Nat::operator/=(const Nat &operand)
{
	Nat result = *this / operand;
	*this = std::move(result);
	return *this;
}

/*! modulus equals */
Nat& Nat::operator%=(const Nat &operand)
{
	Nat result = *this % operand;
	*this = std::move(result);
	return *this;
}


/*--------------------.
| power via squaring. |
`--------------------*/

/*! raise to the power */
Nat Nat::pow(size_t exp) const
{
	if (exp == 0) return 1;
	Nat x = *this, y = 1;
	while (exp > 1) {
		if ((exp & 1) == 0) {
			exp >>= 1;
		} else {
			y *= x;
			exp = (exp - 1) >> 1;
		}
		x *= x;
	}
	return x * y;
}


/*-------------------.
| string conversion. |
`-------------------*/

/*! helper for recursive divide and conquer conversion to string */
static inline ptrdiff_t _to_string_c(const Nat &val, std::string &s, ptrdiff_t offset)
{
	limb2_t v = limb2_t(val.limb_at(0)) | (limb2_t(val.limb_at(1)) << Nat::limb_bits);
	do {
		s[--offset] = '0' + char(v % 10);
	} while ((v /= 10) != 0);
	return offset;
}

/*! helper for recursive divide and conquer conversion to string */
static ptrdiff_t _to_string_r(const Nat &val, std::vector<Nat> &sq, size_t level,
	std::string &s, size_t digits, ptrdiff_t offset)
{
	Nat q, r;
	Nat::divrem(val, sq[level], q, r);
	if (level > 0) {
		if (r != 0) {
			if (q != 0) {
				_to_string_r(r, sq, level-1, s, digits >> 1, offset);
				return _to_string_r(q, sq, level-1, s, digits >> 1, offset - digits);
			} else {
				return _to_string_r(r, sq, level-1, s, digits >> 1, offset);
			}
		}
	} else {
		if (r != 0) {
			if (q != 0) {
				_to_string_c(r, s, offset);
				offset = _to_string_c(q, s, offset - digits);
			} else {
				offset = _to_string_c(r, s, offset);
			}
		}
	}
	return offset;
}

/*! convert from Nat to string */
std::string Nat::to_string(size_t radix) const
{
	static const char* hexdigits = "0123456789abcdef";
	static const Nat tenp18{0xa7640000, 0xde0b6b3};
	static const size_t dgib = 3566893131; /* log2(10) * 1024^3 */

	switch (radix) {
		case 10: {
			if (*this == 0) return "0";

			/* estimate string length */
			std::string s;
			size_t climit = (size_t)(((long long)(num_limbs()) << (limb_shift + 30)) / (long long)dgib) + 1;
			s.resize(climit, '0');

			/* square the chunk size until ~= sqrt(n) */
			Nat chunk = tenp18;
			size_t digits = 18;
			std::vector<Nat> sq = { tenp18 };
			do {
				chunk *= chunk;
				digits <<= 1;
				sq.push_back(chunk);
			} while ((chunk.num_limbs() < ((num_limbs() >> 1) + 1)));

			/* recursively divide by chunk squares */
			ptrdiff_t offset = _to_string_r(*this, sq, sq.size() - 1, s, digits, climit);

			/* return less reserve */
			return s.substr(offset);
		}
		case 2: {
			if (*this == 0) return "0b0";

			std::string s("0b");
			limb_t l1 = limbs.back();
			size_t n = limb_bits - clz(l1);
			size_t t = n + ((num_limbs() - 1) << limb_shift);
			s.resize(t + 2);
			auto i = s.begin() + 2;
			for (ptrdiff_t k = n - 1; k >= 0; k--) {
				*(i++) = '0' + ((l1 >> k) & 1);
			}
			for (ptrdiff_t j = num_limbs() - 2; j >= 0; j--) {
				limb_t l = limbs[j];
				for (ptrdiff_t k = limb_bits - 1; k >= 0; k--) {
					*(i++) = '0' + ((l >> k) & 1);
				}
			}
			return s;
		}
		case 16: {
			if (*this == 0) return "0x0";

			std::string s("0x");
			limb_t l1 = limbs.back();
			size_t n = ((limb_bits >> 2) - (clz(l1) >> 2));
			size_t t = n + ((num_limbs() - 1) << (limb_shift - 2));
			s.resize(t + 2);
			auto i = s.begin() + 2;
			for (ptrdiff_t k = n - 1; k >= 0; k--) {
				*(i++) = hexdigits[(l1 >> (k << 2)) & 0xf];
			}
			for (ptrdiff_t j = num_limbs() - 2; j >= 0; j--) {
				limb_t l = limbs[j];
				for (ptrdiff_t k = (limb_bits >> 2) - 1; k >= 0; k--) {
					*(i++) = hexdigits[(l >> (k << 2)) & 0xf];
				}
			}
			return s;
		}
		default: {
			return std::string();
		}
	}
}

/*! convert to Nat from string */
void Nat::from_string(const char *str, size_t len, size_t radix)
{
	static const Nat tenp18{0xa7640000, 0xde0b6b3};
	static const Nat twop64{0,0,1};
	if (len > 2) {
		if (strncmp(str, "0b", 2) == 0) {
			radix = 2;
			str += 2;
			len -= 2;
		} else if (strncmp(str, "0x", 2) == 0) {
			radix = 16;
			str += 2;
			len -= 2;
		}
	}
	if (radix == 0) {
		radix = 10;
	}
	switch (radix) {
		case 10: {
			for (size_t i = 0; i < len; i += 18) {
				size_t chunklen = i + 18 < len ? 18 : len - i;
				std::string chunk(str + i, chunklen);
				limb2_t num = strtoull(chunk.c_str(), nullptr, 10);
				if (chunklen == 18) {
					*this *= tenp18;
				} else {
					*this *= Nat(10).pow(chunklen);
				}
				*this += Nat{limb_t(num), limb_t(num >> limb_bits)};
			}
			break;
		}
		case 2: {
			for (size_t i = 0; i < len; i += 64) {
				size_t chunklen = i + 64 < len ? 64 : len - i;
				std::string chunk(str + i, chunklen);
				limb2_t num = strtoull(chunk.c_str(), nullptr, 2);
				if (chunklen == 64) {
					*this *= twop64;
				} else {
					*this *= Nat(2).pow(chunklen);
				}
				*this += Nat{limb_t(num), limb_t(num >> limb_bits)};
			}
			break;
		}
		case 16: {
			for (size_t i = 0; i < len; i += 16) {
				size_t chunklen = i + 16 < len ? 16 : len - i;
				std::string chunk(str + i, chunklen);
				limb2_t num = strtoull(chunk.c_str(), nullptr, 16);
				if (chunklen == 16) {
					*this *= twop64;
				} else {
					*this *= Nat(16).pow(chunklen);
				}
				*this += Nat{limb_t(num), limb_t(num >> limb_bits)};
			}
			break;
		}
		default: {
			limbs.push_back(0);
		}
	}
}
