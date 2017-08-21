/*
 * int.h
 *
 * integer with sign and magnitude
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

#include "nat.h"

struct Int
{
	bool sign;
	Nat mag;

	/*! default constructor */
	Int();

	/*! integral constructor */
	Int(int n);

	/*! copy constructor  */
	Int(const Int &o);

	/*! integral copy assignment operator */
	Int& operator=(int n);

	/*! Nat copy assignment operator */
	Int& operator=(const Int &operand);


	/* define self mutating operations */

	/*! add equals */
	Int& operator+=(const Int &operand);

	/*! subtract equals */
	Int& operator-=(const Int &operand);

	/*! multiply equals */
	Int& operator*=(const Int &operand);

	/*! divide equals */
	Int& operator/=(const Int &operand);

	/*! remainder equals */
	Int& operator%=(const Int &operand);

	/* const operations copy and use the mutating operations */

	/*! add */
	Int operator+(const Int &operand) const;

	/*! subtract */
	Int operator-(const Int &operand) const;

	/*! multiply */
	Int operator*(const Int &operand) const;

	/*! divide */
	Int operator/(const Int &operand) const;

	/*! remainder */
	Int operator%(const Int &operand) const;

	/*! negate */
	Int operator-() const;

	/*! not */
	bool operator!() const;

	/*
	 * comparison are defined in terms of "equals" and "less than"
	 */

	/*! equals */
	bool operator==(const Int &operand) const;

	/*! less than */
	bool operator<(const Int &operand) const;

	/*
	 * axiomatically define other comparisons in terms of "equals" and "less than"
	 */

	/*! not equals */
	bool operator!=(const Int &operand) const;

	/*! less than or equal*/
	bool operator<=(const Int &operand) const;

	/*! greater than */
	bool operator>(const Int &operand) const;

	/*! less than or equal*/
	bool operator>=(const Int &operand) const;

	/*
	 * convert natural number to string
	 */

	/*! convert Nat to string */
	std::string to_string() const;
};
