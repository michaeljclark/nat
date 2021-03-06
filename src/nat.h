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

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <limits>
#include <iostream>
#include <algorithm>
#include <initializer_list> 

struct Nat
{
	/*------------------.
	| type definitions. |
	`------------------*/

	/*! configuration flags */
	struct signedness { bool is_signed; };

	static const signedness _unsigned;  /* unsigned */
	static const signedness _signed;    /* signed two's complement */

	/*! limb bit width and bit shift */
	enum {
		limb_bits = 32,
		limb_shift = 5,
	};

	/*! limb type */
	typedef unsigned int limb_t;
	typedef unsigned long long limb2_t;
	typedef signed long long slimb2_t;


	/*------------------.
	| member variables. |
	`------------------*/

	/* limbs is a vector of words with the little end at offset 0 */
	std::vector<limb_t> limbs;

	/*! flags indicating unsigned or signed two's complement */
	signedness s;

	/*! contains the width of the natural in bits (variable width = 0) */
	unsigned bits;


	/*--------------.
	| constructors. |
	`--------------*/

	/*! default constructor */
	Nat(signedness s = _unsigned, unsigned bits = 0);

	/*! integral constructor */
	Nat(const limb_t n, signedness s = _unsigned, unsigned bits = 0);

	/*! array constructor */
	Nat(const std::initializer_list<limb_t> l, signedness s = _unsigned, unsigned bits = 0);

	/*! string constructor */
	Nat(std::string str, signedness s = _unsigned, unsigned bits = 0);

	/*! string constructor with radix */
	Nat(std::string str, size_t radix, signedness s = _unsigned, unsigned bits = 0);

	/*! copy constructor */
	Nat(const Nat &operand);

	/*! move constructor */
	Nat(const Nat&& operand) noexcept;


	/*----------------------.
	| assignment operators. |
	`----------------------*/

	/*! integral copy assignment operator */
	Nat& operator=(const limb_t l);

	/*! Nat copy assignment operator */
	Nat& operator=(const Nat &operand);

	/*! Nat move assignment operator */
	Nat& operator=(Nat &&operand);


	/*------------------.
	| internal methods. |
	`------------------*/

	/*! expand limbs to match operand */
	void _expand(const Nat &operand);

	/*! contract zero big end limbs */
	void _contract();

	/*! resize number of limbs */
	void _resize(size_t n);


	/*-------------------------------.
	| limb and bit accessor methods. |
	`-------------------------------*/

	/*! return number of limbs */
	size_t num_limbs() const;

	/*! return maximum number of limbs */
	size_t max_limbs() const;

	/*! access word at limb offset */
	limb_t limb_at(size_t n) const;

	/*! limb_mask at limb offset */
	limb_t limb_mask(size_t n) const;

	/*! test bit at bit offset */
	int test_bit(size_t n) const;

	/*! set bit at bit offset */
	void set_bit(size_t n);

	/*! return number of bits */
	size_t num_bits() const;

	/*! test sign */
	bool sign_bit() const;


	/*---------------------------------------------.
	| add, subtract, shifts and logical operators. |
	`---------------------------------------------*/

	/*! add with carry equals */
	Nat& operator+=(const Nat &operand);

	/*! subtract with borrow equals */
	Nat& operator-=(const Nat &operand);

	/*! left shift equals */
	Nat& operator<<=(size_t shamt);

	/*! right shift equals */
	Nat& operator>>=(size_t shamt);

	/*! bitwise and equals */
	Nat& operator&=(const Nat &operand);

	/*! bitwise or equals */
	Nat& operator|=(const Nat &operand);

	/*! bitwise xor equals */
	Nat& operator^=(const Nat &operand);

	/*! add with carry */
	Nat operator+(const Nat &operand) const;

	/*! subtract with borrow */
	Nat operator-(const Nat &operand) const;

	/*! left shift */
	Nat operator<<(size_t shamt) const;

	/*! right shift */
	Nat operator>>(size_t shamt) const;

	/*! bitwise and */
	Nat operator&(const Nat &operand) const;

	/*! bitwise or */
	Nat operator|(const Nat &operand) const;

	/*! bitwise xor */
	Nat operator^(const Nat &operand) const;

	/*! bitwise not */
	Nat operator~() const;

	/*! negate */
	Nat operator-() const;


	/*----------------------.
	| comparison operators. |
	`----------------------*/

	/*! equals */
	bool operator==(const Nat &operand) const;

	/*! less than */
	bool operator<(const Nat &operand) const;

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


	/*-------------------------.
	| multply, divide and pow. |
	`-------------------------*/

	/*! base 2^limb_bits multiply */
	static void mult(const Nat &multiplicand, const Nat multiplier, Nat &result);

	/*! base 2^limb_bits division */
	static void divrem(const Nat &dividend, const Nat &divisor, Nat &quotient, Nat &remainder);

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


	/*-------------------.
	| string conversion. |
	`-------------------*/

	/*! convert Nat to string */
	std::string to_string(size_t radix = 10) const;

	/*! convert Nat from string */
	void from_string(const char *str, size_t len, size_t radix);

};
