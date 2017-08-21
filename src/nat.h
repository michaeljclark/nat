/*
 * nat.h
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

#pragma once

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <limits>
#include <iostream>
#include <algorithm>
#include <initializer_list>

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


	/*
	 * constructors
	 */

	/*! default constructor */
	Nat();

	/*! integral constructor */
	Nat(const limb_t n);

	/*! array constructor */
	Nat(const std::initializer_list<limb_t> l);

	/*! string constructor */
	Nat(std::string str, size_t radix = 0);

	/*! copy constructor  */
	Nat(const Nat &operand);

	/*! move constructor  */
	Nat(const Nat&& operand) noexcept;


	/*
	 * assignment operators
	 */

	/*! integral copy assignment operator */
	Nat& operator=(const limb_t l);

	/*! Nat copy assignment operator */
	Nat& operator=(const Nat &operand);

	/*! Nat move assignment operator */
	Nat& operator=(Nat &&operand);


	/*
	 * internal methods
	 */

	/*! expand limbs to match operand */
	void _expand(const Nat &operand);

	/*! contract zero big end limbs */
	void _contract();

	/*! resize number of limbs */
	void _resize(size_t n);


	/*
	 * handy limb and bit accessor methods
	 */

	/*! return number of limbs */
	size_t num_limbs() const;

	/*! access word at limb offset */
	limb_t limb_at(size_t n) const;

	/*! test bit at bit offset */
	int test_bit(size_t n) const;

	/*! set bit at bit offset */
	void set_bit(size_t n);


	/* 
	 * multiply and divide require add with carry, subtract  
	 * with borrow, left and right shift logical operators
	 */


	/* define self mutating operations */

	/*! add with carry equals */
	Nat& operator+=(const Nat &operand);

	/*! subtract with borrow equals */
	Nat& operator-=(const Nat &operand);

	/*! left shift equals */
	Nat& operator<<=(int shamt);

	/*! right shift equals */
	Nat& operator>>=(int shamt);

	/*! logical and equals */
	Nat& operator&=(const Nat &operand);

	/*! logical or equals */
	Nat& operator|=(const Nat &operand);


	/* const operations copy and use the mutating operations */

	/*! add with carry */
	Nat operator+(const Nat &operand) const;

	/*! subtract with borrow */
	Nat operator-(const Nat &operand) const;
	/*! left shift */
	Nat operator<<(int shamt) const;

	/*! right shift */
	Nat operator>>(int shamt) const;

	/*! logical and */
	Nat operator&(const Nat &operand) const;

	/*! logical or */
	Nat operator|(const Nat &operand) const;


	/*
	 * comparison are defined in terms of "equals" and "less than"
	 */

	/*! equals */
	bool operator==(const Nat &operand) const;

	/*! less than */
	bool operator<(const Nat &operand) const;


	/*
	 * axiomatically define other comparisons in terms of "equals" and "less than"
	 */

	/*! not equals */
	bool operator!=(const Nat &operand) const;

	/*! less than or equal*/
	bool operator<=(const Nat &operand) const;

	/*! greater than */
	bool operator>(const Nat &operand) const;

	/*! less than or equal*/
	bool operator>=(const Nat &operand) const;

	/*! not */
	bool operator!() const;

	/*
	 * multply, divide, modulus and pow
	 *
	 * These routines are derived from Hacker's Delight
	 */

	/*! base 2^limb_bits multiply */
	void mult(const Nat &multiplicand, const Nat multiplier, Nat &result) const;

	/*! base 2^limb_bits division */
	void divrem(const Nat &divisor, Nat &quotient, Nat &remainder) const;

	/*! multiply */
	Nat operator*(const Nat &operand) const;

	/*! division quotient */
	Nat operator/(const Nat &divisor) const;

	/*! division remainder */
	Nat operator%(const Nat &divisor) const;

	/*! multiply equals */
	Nat& operator*=(const Nat &operand);

	/*! divide equals */
	Nat& operator/=(const Nat &operand);

	/*! modulus equals */
	Nat& operator%=(const Nat &operand);

	/*! raise to the power */
	Nat pow(size_t exp) const;


	/*
	 * convert natural number to string
	 */

	/*! convert Nat to string */
	std::string to_string(size_t radix = 10) const;

	void from_string(const char *str, size_t len, size_t radix);

};
