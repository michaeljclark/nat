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

struct Int
{
	bool sign;
	Nat mag;

	/*! default constructor */
	Int() : sign(0), mag(0) {}

	/*! integral constructor */
	Int(int n) : sign(n < 0), mag(n < 0 ? -n : n) {}

	/*! copy constructor  */
	Int(const Int &o) : sign(o.sign), mag(o.mag) {}

	/*! integral copy assignment operator */
	Int& operator=(int n)
	{
		sign = n < 0;
		mag = n < 0 ? -n : n;
		return *this;
	}

	/*! Nat copy assignment operator */
	Int& operator=(const Int &operand)
	{
		sign = operand.sign;
		mag = operand.mag;
		return *this;
	}


	/* define self mutating operations */

	/*! add equals */
	Int& operator+=(const Int &operand)
	{
		if (sign ^ operand.sign) {
			if (this->mag > operand.mag) {
				mag -= operand.mag;
			} else {
				mag = operand.mag - mag;
				sign = operand.sign;
			}
		} else {
			mag += operand.mag;
		}
		return *this;
	}

	/*! subtract equals */
	Int& operator-=(const Int &operand)
	{
		if (sign ^ operand.sign) {
			mag += operand.mag;
		} else {
			if (this->mag > operand.mag) {
				mag -= operand.mag;
			} else {
				mag = operand.mag - mag;
				sign = !operand.sign;
			}
		}
		return *this;
	}

	/*! multiply equals */
	Int& operator*=(const Int &operand)
	{
		mag *= operand.mag;
		sign ^= operand.sign;
		return *this;
	}

	/*! divide equals */
	Int& operator/=(const Int &operand)
	{
		mag /= operand.mag;
		sign ^= operand.sign;
		return *this;
	}

	/*! remainder equals */
	Int& operator%=(const Int &operand)
	{
		mag %= operand.mag;
		return *this;
	}

	/* const operations copy and use the mutating operations */

	/*! add */
	Int operator+(const Int &operand) const
	{
		Int result(*this);
		return result += operand;
	}

	/*! subtract */
	Int operator-(const Int &operand) const
	{
		Int result(*this);
		return result -= operand;
	}

	/*! multiply */
	Int operator*(const Int &operand) const
	{
		Int result(*this);
		return result *= operand;
	}

	/*! divide */
	Int operator/(const Int &operand) const
	{
		Int result(*this);
		return result /= operand;
	}

	/*! remainder */
	Int operator%(const Int &operand) const
	{
		Int result(*this);
		return result %= operand;
	}

	/*! negate */
	Int operator-() const
	{
		Int result(*this);
		result.sign = !sign;
		return result;
	}


	/*
	 * comparison are defined in terms of "equals" and "less than"
	 */

	/*! equals */
	bool operator==(const Int &operand) const
	{
		return (sign == operand.sign && mag == operand.mag) || (mag == 0 && operand.mag == 0);
	}

	/*! less than */
	bool operator<(const Int &operand) const
	{
		if (sign ^ operand.sign) {
			return sign;
		} else if (sign) {
			return operand.mag < mag;
		} else {
			return mag < operand.mag;
		}
		return false;
	}

	/*
	 * axiomatically define other comparisons in terms of "equals" and "less than"
	 */

	/*! not equals */
	bool operator!=(const Int &operand) const { return !(*this == operand); }

	/*! less than or equal*/
	bool operator<=(const Int &operand) const { return *this < operand || *this == operand; }

	/*! greater than */
	bool operator>(const Int &operand) const { return !(*this <= operand); }

	/*! less than or equal*/
	bool operator>=(const Int &operand) const { return !(*this < operand) || *this == operand; }

};
