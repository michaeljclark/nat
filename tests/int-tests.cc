/*
 * int-tests.cc
 *
 * simple test cases for signed integer implementation
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
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <initializer_list>

#include "nat.h"
#include "int.h"

int main(int argc, char const *argv[])
{
	/* test equal */
	assert(Int(0) == Int(0));
	assert(Int(1) == Int(1));

	/* test not equal */
	assert(Int(1) != Int(0));
	assert(Int(1) != Int(-1));

	/* test greater than */
	assert(Int(2) > Int(1));
	assert(Int(1) > Int(0));
	assert(Int(1) > Int(-1));
	assert(Int(-1) > Int(-2));

	/* test less than */
	assert(Int(1) < Int(2));
	assert(Int(0) < Int(1));
	assert(Int(-1) < Int(1));
	assert(Int(-2) < Int(-1));

	/* test add */
	assert(Int(1) + Int(2) == Int(3));
	assert(Int(3) + Int(-2) == Int(1));
	assert(Int(1) + Int(-2) == Int(-1));
	assert(Int(-2) + Int(-1) == Int(-3));
	assert(Int(-2) + Int(1) == Int(-1));
	assert(Int(-2) + Int(3) == Int(1));

	/* test subtract */
	assert(Int(1) - Int(2) == Int(-1));
	assert(Int(3) - Int(-2) == Int(5));
	assert(Int(1) - Int(-2) == Int(3));
	assert(Int(-2) - Int(-1) == Int(-1));
	assert(Int(-2) - Int(1) == Int(-3));
	assert(Int(-2) - Int(3) == Int(-5));

	/* test multiply */
	assert(Int(1) * Int(2) == Int(2));
	assert(Int(3) * Int(-2) == Int(-6));
	assert(Int(1) * Int(-2) == Int(-2));
	assert(Int(-2) * Int(-1) == Int(2));
	assert(Int(-2) * Int(1) == Int(-2));
	assert(Int(-2) * Int(3) == Int(-6));

	/* test negate */
	assert(-Int(2) == Int(-2));
	assert(-Int(-2) == Int(2));

	/* test not */
	assert(!Int(0));
	assert(!!Int(1));
	assert(!!Int(-1));
}