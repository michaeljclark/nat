/*
 * nat.h
 *
 * unsigned natural number header implementation
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

#pragma once

struct Nat
{
	/*! limb type */
	typedef unsigned int limb_t;
	typedef unsigned long long limb2_t;
	typedef signed long long slimb2_t;

	/*! limb bit width and bit shift */
	enum { limb_bits = 32, limb_shift = 5 };

	/*!
	 * limbs is a vector of words with the little end at offset 0
	 */
	std::vector<limb_t> limbs;

	/*! expand limbs to match operand */
	void expand(const Nat &operand)
	{
		limbs.resize(std::max(limbs.size(), operand.limbs.size()));
	}

	/*! contract zero big end limbs */
	void contract()
	{
		while(limbs.size() > 1 && limbs.back() == 0) {
			limbs.pop_back();
		}
	}

	/*
	 * constructors
	 */

	/*! default constructor */
	Nat() : limbs{ 0 } {}

	/*! integral constructor */
	Nat(limb_t n) : limbs{ n } {}

	/*! array constructor */
	Nat(std::initializer_list<limb_t> l) : limbs(l) { contract(); }

	/*! copy constructor  */
	Nat(const Nat &operand) : limbs(operand.limbs) { contract(); }

	/*! move constructor  */
	Nat(const Nat&& operand) noexcept : limbs(std::move(operand.limbs)) { contract(); }


	/*
	 * handy limb and bit accessor methods
	 */

	/*! return number of limbs */
	size_t num_limbs() const { return limbs.size(); }

	/*! access word at limb offset */
	limb_t limb_at(size_t n) const { return n < limbs.size() ? limbs[n] : 0; }

	/*! test bit at bit offset */
	int test_bit(size_t n) const
	{
		size_t word = n >> limb_shift;
		if (word < limbs.size()) return (limbs[word] >> (n & (limb_bits-1))) & 1;
		else return 0;
	}

	/*! set bit at bit offset */
	void set_bit(size_t n)
	{
		size_t word = n >> limb_shift;
		if (word >= limbs.size()) limbs.resize(word + 1);
		limbs[word] |= (1ULL << (n & (limb_bits-1)));
	}


	/* 
	 * multiply and divide require add with carry, subtract  
	 * with borrow, left and right shift logical operators
	 */


	/* define self mutating operations */

	/*! integral copy assignment operator */
	Nat& operator=(const limb_t l)
	{
		limbs.resize(0);
		limbs.push_back(l);
		return *this;
	}

	/*! Nat copy assignment operator */
	Nat& operator=(const Nat &operand)
	{
		limbs = operand.limbs;
		contract();
		return *this;
	}

	/*! add with carry equals */
	Nat& operator+=(const Nat &operand)
	{
		expand(operand);
		limb_t carry = 0;
		for (size_t i = 0; i < limbs.size(); i++) {
			limb_t old_val = limbs[i];
			limb_t new_val = old_val + operand.limb_at(i) + carry;
			limbs[i] = new_val;
			carry = new_val < old_val;
		}
		if (carry) {
			limbs.push_back(1);
		}
		return *this;
	}

	/*! subtract with borrow equals */
	Nat& operator-=(const Nat &operand)
	{
		expand(operand);
		limb_t borrow = 0;
		for (size_t i = 0; i < limbs.size(); i++) {
			limb_t old_val = limbs[i];
			limb_t new_val = old_val - operand.limb_at(i) - borrow;
			limbs[i] = new_val;
			borrow = new_val > old_val;
		}
		assert(borrow == 0); /* unsigned underflow */
		contract();
		return *this;
	}

	/*! left shift equals */
	Nat& operator<<=(int shamt)
	{
		size_t limb_shamt = shamt >> limb_shift;
		if (limb_shamt > 0) {
			limbs.insert(limbs.begin(), limb_shamt, 0);
			shamt -= limb_shamt;
		}
		if (!shamt) return *this;

		limb_t carry = 0;
		for (size_t j = 0; j < limbs.size(); j++) {
			limb_t old_val = limbs[j];
			limb_t new_val = (old_val << shamt) | carry;
			limbs[j] = new_val;
			carry = old_val >> (limb_bits - shamt);
		}
		if (carry) {
			limbs.push_back(carry);
		}
		return *this;
	}

	/*! right shift equals */
	Nat& operator>>=(int shamt)
	{
		size_t limb_shamt = shamt >> limb_shift;
		if (limb_shamt > 0) {
			limbs.erase(limbs.begin(), limbs.begin() + limb_shamt);
			shamt -= limb_shamt;
		}
		if (!shamt) return *this;

		limb_t carry = 0;
		for (size_t j = limbs.size(); j > 0; j--) {
			limb_t old_val = limbs[j - 1];
			limb_t new_val = (old_val >> shamt) | carry;
			limbs[j - 1] = new_val;
			carry = old_val << (limb_bits - shamt);
		}
		contract();
		return *this;
	}

	/*! logical and equals */
	Nat& operator&=(const Nat &operand)
	{
		expand(operand);
		for (size_t i = 0; i < limbs.size(); i++) {
			limbs[i] = operand.limb_at(i) & limbs[i];
		}
		contract();
		return *this;
	}

	/*! logical or equals */
	Nat& operator|=(const Nat &operand)
	{
		expand(operand);
		for (size_t i = 0; i < limbs.size(); i++) {
			limbs[i] = operand.limb_at(i) | limbs[i];
		}
		contract();
		return *this;
	}


	/* const operations copy and use the mutating operations */

	/*! add with carry */
	Nat operator+(const Nat &operand) const
	{
		Nat result(*this);
		return result += operand;
	}

	/*! subtract with borrow */
	Nat operator-(const Nat &operand) const
	{
		Nat result(*this);
		return result -= operand;
	}

	/*! left shift */
	Nat operator<<(int shamt) const
	{
		Nat result(*this);
		return result <<= shamt;
	}

	/*! right shift */
	Nat operator>>(int shamt) const
	{
		Nat result(*this);
		return result >>= shamt;
	}

	/*! logical and */
	Nat operator&(const Nat &operand) const
	{
		Nat result(*this);
		return result &= operand;
	}

	/*! logical or */
	Nat operator|(const Nat &operand) const
	{
		Nat result(*this);
		return result |= operand;
	}


	/*
	 * comparison are defined in terms of "equals" and "less than"
	 */

	/*! equals */
	bool operator==(const Nat &operand) const
	{
		size_t max = std::max(num_limbs(), operand.num_limbs());
		for (size_t i = 0; i < max; i++) {
			if (limb_at(i) != operand.limb_at(i)) return false;
		}
		return true;
	}

	/*! less than */
	bool operator<(const Nat &operand) const
	{
		size_t max = std::max(num_limbs(), operand.num_limbs());
		for (size_t i = max; i > 0; i--) {
			if (limb_at(i - 1) > operand.limb_at(i - 1)) return false;
			else if (limb_at(i - 1) < operand.limb_at(i - 1)) return true;
		}
		return false;
	}


	/*
	 * axiomatically define other comparisons in terms of "equals" and "less than"
	 */

	/*! not equals */
	bool operator!=(const Nat &operand) const { return !(*this == operand); }

	/*! less than or equal*/
	bool operator<=(const Nat &operand) const { return *this < operand || *this == operand; }

	/*! greater than */
	bool operator>(const Nat &operand) const { return !(*this <= operand); }

	/*! less than or equal*/
	bool operator>=(const Nat &operand) const { return !(*this < operand) || *this == operand; }


	/*
	 * multply, divide, modulus and pow
	 *
	 * These routines are derived from Hacker's Delight
	 */

	/*! base 2^limb_bits multiply */
	void mult(const Nat &multiplicand, const Nat multiplier, Nat &result) const
	{
		size_t m = multiplicand.limbs.size(), n = multiplier.limbs.size();
		result.limbs.resize(m + n);
		for (size_t j = 0; j < n; j++) {
			limb_t k = 0;
			for (size_t i = 0; i < m; i++) {
				limb2_t t = limb2_t(multiplicand.limbs[i]) * limb2_t(multiplier.limbs[j]) +
					limb2_t(result.limbs[i + j]) + limb2_t(k);
				result.limbs[i + j] = limb_t(t);
				k = t >> limb_bits;
			}
			result.limbs[j + m] = k;
		}
	}

	/*! base 2^limb_bits division */
	void divrem(const Nat &divisor, Nat &quotient, Nat &remainder) const
	{
		quotient = 0;
		remainder = 0;
		size_t m = limbs.size(), n = divisor.limbs.size();
		quotient.limbs.resize(std::max(m - n + 1, size_t(1)));
		remainder.limbs.resize(n);
		limb_t *q = quotient.limbs.data(), *r = remainder.limbs.data();
		const limb_t *u = limbs.data(), *v = divisor.limbs.data();

		const limb2_t b = (1ULL << limb_bits); // Number base
		limb_t *un, *vn;                       // Normalized form of u, v.
		limb2_t qhat;                          // Estimated quotient digit.
		limb2_t rhat;                          // A remainder.

		if (m < n || n <= 0 || v[n-1] == 0) {
			return; // Return if invalid param.
		}

		// Single digit divisor
		if (n == 1) {
			limb2_t k = 0;
			for (ssize_t j = m - 1; j >= 0; j--) {
				q[j] = limb_t((k*b + u[j]) / v[0]);
				k = (k*b + u[j]) - q[j]*v[0];
			}
			r[0] = limb_t(k);
			return;
		}

		// Normalize by shifting v left just enough so that
		// its high-order bit is on, and shift u left the
		// same amount. We may have to append a high-order
		// digit on the dividend; we do that unconditionally.

		int s = __builtin_clz(v[n-1]); // 0 <= s <= limb_bits.
		vn = (limb_t *)alloca(sizeof(limb_t) * n);
		for (size_t i = n - 1; i > 0; i--) {
			vn[i] = (v[i] << s) | (v[i-1] >> (limb_bits-s));
		}
		vn[0] = v[0] << s;

		un = (limb_t *)alloca(sizeof(limb_t) * (m + 1));
		un[m] = u[m-1] >> (limb_bits-s);
		for (size_t i = m - 1; i > 0; i--) {
			un[i] = (u[i] << s) | (u[i-1] >> (limb_bits-s));
		}
		un[0] = u[0] << s;
		for (ssize_t j = m - n; j >= 0; j--) { // Main loop.
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
			for (size_t i = 0; i < n; i++) {
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
				for (size_t i = 0; i < n; i++) {
					t = un[i+j] + vn[i] + k;
					un[i+j] = limb_t(t);
					k = t >> limb_bits;
				}
				un[j+n] = limb_t(un[j+n] + k);
			}
		}

		// normalize remainder
		for (size_t i = 0; i < n; i++) {
			r[i] = (un[i] >> s) | (un[i + 1] << (limb_bits - s));
		}

		quotient.contract();
		remainder.contract();
	}

	/*! multiply */
	Nat operator*(const Nat &operand) const
	{
		Nat result(0);
		mult(*this, operand, result);
		return result;
	}

	/*! division quotient */
	Nat operator/(const Nat &divisor) const
	{
		Nat quotient(0), remainder(0);
		divrem(divisor, quotient, remainder);
		return quotient;
	}

	/*! division remainder */
	Nat operator%(const Nat &divisor) const
	{
		Nat quotient(0), remainder(0);
		divrem(divisor, quotient, remainder);
		return remainder;
	}

	/*! multiply equals */
	Nat& operator*=(const Nat &operand)
	{
		Nat result = *this * operand;
		limbs = result.limbs;
		return *this;
	}

	/*! divide equals */
	Nat& operator/=(const Nat &operand)
	{
		Nat result = *this / operand;
		limbs = result.limbs;
		return *this;
	}

	/*! modulus equals */
	Nat& operator%=(const Nat &operand)
	{
		Nat result = *this % operand;
		limbs = result.limbs;
		return *this;
	}

	/*! raise to the power */
	Nat pow(size_t exp) const
	{
		if (exp == 0) return 1;
		Nat x = *this, y = 1;
		while (exp > 1) {
			if ((exp & 1) == 0) {
				exp >>= 1;
			} else {
				y = x * y;
				exp = (exp - 1) >> 1;
			}
			x = x * x;
		}
		return x * y;
	}


	/*
	 * convert natural number to string
	 */

	 /*! convert Nat to string */
	std::string to_string() const
	{
		static const Nat tenp18{0xa7640000, 0xde0b6b3};

		/* estimate string length */
		std::string s;
		size_t climit = 10 * limbs.size() + 1, offset = climit;
		s.resize(climit, '0');

		/* print chunks of 18 digits */
		Nat q, r;
		Nat val = *this;
		while (val > tenp18) {
			val.divrem(tenp18, q, r);
			val = q;
			limb2_t v = limb2_t(r.limb_at(0)) | (limb2_t(r.limb_at(1)) << limb_bits);
			size_t next = offset - 18;
			do {
				s[--offset] = '0' + char(v % 10);
			} while (v /= 10);
			offset = next;
		}

		/* remaining chunk */
		limb2_t v = limb2_t(val.limb_at(0)) | (limb2_t(val.limb_at(1)) << limb_bits);
		do {
			s[--offset] = '0' + char(v % 10);
		} while (v /= 10);

		/* return less reserve */
		return s.substr(offset);
	}
};
