/*
 * int.cc
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

#include "int.h"

/*! default constructor */
Int::Int() : sign(0), mag(0) {}

/*! integral constructor */
Int::Int(int n) : sign(n < 0), mag(n < 0 ? -n : n) {}

/*! string constructor */
Int::Int(std::string str) : sign(0), mag(0) { from_string(str.c_str(), str.size()); }

/*! copy constructor  */
Int::Int(const Int &o) : sign(o.sign), mag(o.mag) {}

/*! integral copy assignment operator */
Int& Int::operator=(int n)
{
	sign = n < 0;
	mag = n < 0 ? -n : n;
	return *this;
}

/*! Nat copy assignment operator */
Int& Int::operator=(const Int &operand)
{
	sign = operand.sign;
	mag = operand.mag;
	return *this;
}


/* define self mutating operations */

/*! add equals */
Int& Int::operator+=(const Int &operand)
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
Int& Int::operator-=(const Int &operand)
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
Int& Int::operator*=(const Int &operand)
{
	mag *= operand.mag;
	sign ^= operand.sign;
	return *this;
}

/*! divide equals */
Int& Int::operator/=(const Int &operand)
{
	mag /= operand.mag;
	sign ^= operand.sign;
	return *this;
}

/*! remainder equals */
Int& Int::operator%=(const Int &operand)
{
	mag %= operand.mag;
	return *this;
}

/* const operations copy and use the mutating operations */

/*! add */
Int Int::operator+(const Int &operand) const
{
	Int result(*this);
	return result += operand;
}

/*! subtract */
Int Int::operator-(const Int &operand) const
{
	Int result(*this);
	return result -= operand;
}

/*! multiply */
Int Int::operator*(const Int &operand) const
{
	Int result(*this);
	return result *= operand;
}

/*! divide */
Int Int::operator/(const Int &operand) const
{
	Int result(*this);
	return result /= operand;
}

/*! remainder */
Int Int::operator%(const Int &operand) const
{
	Int result(*this);
	return result %= operand;
}

/*! negate */
Int Int::operator-() const
{
	Int result(*this);
	result.sign = !sign;
	return result;
}

/*! not */
bool Int::operator!() const
{
	return mag == 0;
}

/*! raise to the power */
Int Int::pow(size_t exp) const
{
	Int result;
	result.mag = mag.pow(exp);
	return result;
}


/*
 * comparison are defined in terms of "equals" and "less than"
 */

/*! equals */
bool Int::operator==(const Int &operand) const
{
	return (sign == operand.sign && mag == operand.mag) || (mag == 0 && operand.mag == 0);
}

/*! less than */
bool Int::operator<(const Int &operand) const
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
bool Int::operator!=(const Int &operand) const { return !(*this == operand); }

/*! less than or equal*/
bool Int::operator<=(const Int &operand) const { return *this < operand || *this == operand; }

/*! greater than */
bool Int::operator>(const Int &operand) const { return !(*this <= operand); }

/*! less than or equal*/
bool Int::operator>=(const Int &operand) const { return !(*this < operand) || *this == operand; }

/*
 * convert natural number to string
 */

/*! convert Nat to string */
std::string Int::to_string() const
{
	if (sign) {
		return std::string("-") + mag.to_string();
	}
	else {
		return mag.to_string();
	}
}

/*! convert Nat from string */
void Int::from_string(const char *str, size_t len)
{
	if (len == 0) return;
	if (*str == '-') {
		sign = true;
		mag = Nat(std::string(str + 1, len - 1));
	} else {
		mag = Nat(std::string(str, len));
	}
}

