/*
 * nat-tests.cc
 *
 * simple test cases for unsigned natural number implementation
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

int main(int argc, char const *argv[])
{
	/* test empty constructor */
	Nat b1;
	assert(b1.num_limbs() == 1);
	assert(b1.limb_at(0) == 0);

	/* test integral constructor */
	Nat b2(1);
	assert(b2.num_limbs() == 1);
	assert(b2.limb_at(0) == 1);

	/* test array constructor */
	Nat b3{1, 1};
	assert(b3.num_limbs() == 2);
	assert(b3.limb_at(0) == 1);
	assert(b3.limb_at(1) == 1);

	/* test left shift */
	Nat b4 = 1;
	b4 = b4 << 1;
	assert(b4.num_limbs() == 1);
	assert(b4.limb_at(0) == 2);

	/* test left shift carry overflow */
	Nat b5(std::numeric_limits<Nat::limb_t>::max());
	b5 = b5 << 10;
	assert(b5.num_limbs() == 2);
	assert(b5.limb_at(0) == std::numeric_limits<Nat::limb_t>::max() - 1023);
	assert(b5.limb_at(1) == 1023);

	/* test big left shift */
	Nat b6 = Nat{1, 1} << 2;
	assert(b6.num_limbs() == 2);
	assert(b6.limb_at(0) == 4);
	assert(b6.limb_at(1) == 4);

	/* test right shift carry under */
	assert((Nat{0,1} >> 1) == Nat(1ULL << (Nat::limb_bits - 1)));

	/* test right shift carry under limb contraction */
	assert((Nat{0,1} >> 1).num_limbs() == 1);

	/* test big right shift */
	assert((Nat{0,1} >> 17) == Nat(1ULL << (Nat::limb_bits - 17)));

	/* test logical and */
	Nat b7 = Nat{0b101, 0b101} & Nat{0b100, 0};
	assert(b7.num_limbs() == 1);
	assert(b7.limb_at(0) == 0b100);

	/* test logical or */
	Nat b8 = Nat{0b101, 0b101} | Nat{0b100, 0};
	assert(b8.num_limbs() == 2);
	assert(b8.limb_at(0) == 0b101);
	assert(b8.limb_at(1) == 0b101);

	/* test equals */
	assert((Nat{2,3} == Nat{2,3,0}));
	assert((Nat{2,3,0} == Nat{2,3}));
	assert((Nat{1,2,3} == Nat{1,2,3}));
	assert(!(Nat{1,2,3} == Nat{3,2,3}));
	assert(!(Nat{1,2,3} == 0));
	assert(!(Nat{1,2,3} == 2147483648));

	/* test not equals */
	assert(!(Nat{2,3} != Nat{2,3,0}));
	assert(!(Nat{2,3,0} != Nat{2,3}));
	assert(!(Nat{1,2,3} != Nat{1,2,3}));
	assert((Nat{1,2,3} != Nat{3,2,3}));
	assert((Nat{1,2,3} != 0));
	assert((Nat{1,2,3} != 2147483648));

	/* test less than */
	assert(!(Nat{3,2,3} < Nat{1,2,3}));
	assert(!(Nat{1,2,3} < Nat{1,2,3}));
	assert((Nat{1,2,3} < Nat{3,2,3}));
	assert((Nat{1,2,3} < Nat{3,2,3,1}));
	assert(!(Nat{3,2,3,1} < Nat{1,2,3}));
	assert(!(Nat{3,2,3,1} < 0));
	assert(!(Nat{3,2,3,1} < 2147483648));

	/* test less than equals */
	assert(!(Nat{3,2,3} <= Nat{1,2,3}));
	assert((Nat{1,2,3} <= Nat{1,2,3}));
	assert((Nat{1,2,3} <= Nat{3,2,3}));
	assert((Nat{1,2,3} <= Nat{3,2,3,1}));
	assert(!(Nat{3,2,3,1} <= Nat{1,2,3}));
	assert(!(Nat{3,2,3,1} <= 0));
	assert(!(Nat{3,2,3,1} <= 2147483648));

	/* test greater than */
	assert((Nat{3,2,3} > Nat{1,2,3}));
	assert(!(Nat{1,2,3} > Nat{1,2,3}));
	assert(!(Nat{1,2,3} > Nat{3,2,3}));
	assert(!(Nat{1,2,3} > Nat{3,2,3,1}));
	assert((Nat{3,2,3,1} > Nat{1,2,3}));
	assert((Nat{3,2,3,1} > 0));
	assert((Nat{3,2,3,1} > 2147483648));

	/* test greater than equals */
	assert((Nat{3,2,3} >= Nat{1,2,3}));
	assert((Nat{1,2,3} >= Nat{1,2,3}));
	assert(!(Nat{1,2,3} >= Nat{3,2,3}));
	assert(!(Nat{1,2,3} >= Nat{3,2,3,1}));
	assert((Nat{3,2,3,1} >= Nat{1,2,3}));
	assert((Nat{3,2,3,1} >= 0));
	assert((Nat{3,2,3,1} >= 2147483648));

	/* test not */
	assert(!Nat(0));
	assert(!!Nat(1));

	/* test addition */
	assert((Nat{1,1,1} + Nat{1,1,1} == Nat{2,2,2}));

	/* test addition with carry */
	Nat b9{0,std::numeric_limits<Nat::limb_t>::max()};
	Nat b10{0,std::numeric_limits<Nat::limb_t>::max()-1,1};
	assert(b9 + b9 == b10);

	/* test multiplication */
	Nat b12 = Nat(2147483648) * Nat(2147483648);
	assert(b12.num_limbs() == 2);
	assert(b12.limb_at(0) == 0);
	assert(b12.limb_at(1) == 1073741824);
	Nat b13 = b12 * b12;
	assert(b13.num_limbs() == 4);
	assert(b13.limb_at(0) == 0);
	assert(b13.limb_at(1) == 0);
	assert(b13.limb_at(2) == 0);
	assert(b13.limb_at(3) == 268435456);
	Nat b14 = Nat(2147483647) * Nat(2147483647);
	assert(b14.num_limbs() == 2);
	assert(b14.limb_at(0) == 1);
	assert(b14.limb_at(1) == 1073741823);
	Nat b15 = b14 * b14;
	assert(b15.num_limbs() == 4);
	assert(b15.limb_at(0) == 1);
	assert(b15.limb_at(1) == 2147483646);
	assert(b15.limb_at(2) == 2147483649);
	assert(b15.limb_at(3) == 268435455);

	/* test subtraction */
	assert((Nat{3,3,3} - Nat{1,1,1} == Nat{2,2,2}));

	/* test subtraction with borrow */
	Nat b17{1,1};
	Nat b18{std::numeric_limits<Nat::limb_t>::max()};
	assert(b17-b18 == 2);

	/* test division */
	Nat b19 = b15 / b14;
	assert(b19.num_limbs() == 2);
	assert(b19.limb_at(0) == 1);
	assert(b19.limb_at(1) == 1073741823);
	assert(b19.to_string() == "4611686014132420609");

	/* test set and test bit */
	Nat b20;
	b20.set_bit(64);
	assert(b20.test_bit(64) == 1);
	assert(b20.test_bit(63) == 0);
	b20 = b20 - 1;
	assert(b20.test_bit(64) == 0);
	assert(b20.test_bit(63) == 1);

	/* decimal string formatting */
	assert((Nat(9) * Nat(9)).to_string() == "81");
	assert((Nat(255) * Nat(255)).to_string() == "65025");
	assert((Nat(65535) * Nat(65535)).to_string() == "4294836225");
	assert((Nat(2147483648) * Nat(2147483648)).to_string() == "4611686018427387904");

	/* binary string formatting */
	assert((Nat{0b101}).to_string(2) == "0b101");
	assert((Nat{0b111100001111}).to_string(2) == "0b111100001111");
	assert((Nat{0xff00ff,0xff}).to_string(2) == "0b1111111100000000111111110000000011111111");

	/* hex string formatting */
	assert((Nat{0x1}).to_string(16) == "0x1");
	assert((Nat{0x7f}).to_string(16) == "0x7f");
	assert((Nat{0x3ff}).to_string(16) == "0x3ff");
	assert((Nat{0xffffffff,1}).to_string(16) == "0x1ffffffff");
	assert((Nat{0xffffffff,0x80}).to_string(16) == "0x80ffffffff");
	assert((Nat{0xffffffff,0x400}).to_string(16) == "0x400ffffffff");
	assert((Nat{0x80000000,0x80000000}).to_string(16) == "0x8000000080000000");

	/* pow */
	assert(Nat(71).pow(0) == 1);
	assert(Nat(71).pow(1) == 71);
	assert(Nat(71).pow(17).to_string() == "29606831241262271996845213307591");

	/* from string */
	assert(Nat("71").to_string() == "71");
	assert(Nat("29606831241262271996845213307591").to_string() == "29606831241262271996845213307591");
	assert(Nat("0xdeadbeef").to_string(16) == "0xdeadbeef");
	assert(Nat("0xdeadbeef00ff00ff00ff00ff").to_string(16) == "0xdeadbeef00ff00ff00ff00ff");
	assert(Nat("0b101").to_string(2) == "0b101");
	assert(Nat("0b11110000111100001111000011110000").to_string(2) == "0b11110000111100001111000011110000");
	assert(Nat("3249094308290873429032409832424398902348094329803249083249089802349809430822903").to_string()
		== "3249094308290873429032409832424398902348094329803249083249089802349809430822903");

	/* fixed width tests */
	assert(Nat(0xffffffff, Nat::_unsigned, 32) + 2 == 1);
	assert(Nat(0xffffffff, Nat::_unsigned, 31) == 0x7fffffff);
	assert(Nat(0x7fffffff, Nat::_unsigned, 31) + 2 == 1);
	assert(Nat("0xffffffff", Nat::_unsigned, 31) == 0x7fffffff);
	assert(Nat(100000) * Nat(100000) == (Nat{0x540be400, 0x2}));
	assert(Nat(100000, Nat::_unsigned, 34) * Nat(100000, Nat::_unsigned, 34) == (Nat{0x540be400, 0x2}));
	assert(Nat(100000, Nat::_unsigned, 33) * Nat(100000, Nat::_unsigned, 33) == Nat(0x540be400));
	assert(Nat(100000, Nat::_unsigned, 32) * Nat(100000, Nat::_unsigned, 32) == Nat(0x540be400));
	assert(Nat(100000, Nat::_unsigned, 20) * Nat(100000, Nat::_unsigned, 20) == Nat(0xbe400));
	assert(-Nat(1, Nat::_unsigned, 32) == Nat(-1, Nat::_unsigned, 32));
	assert(-Nat(1, Nat::_unsigned, 64) == Nat({0xffffffff, 0xffffffff}, Nat::_unsigned, 64));
	assert(-Nat(1, Nat::_unsigned, 65) == Nat({0xffffffff, 0xffffffff,1}, Nat::_unsigned, 65));
	assert(Nat({0xffffffff, 0x7fffffff,1}, Nat::_unsigned, 65) >> 1 == Nat({0xffffffff, 0xbfffffff}, Nat::_unsigned, 65));
	assert(-Nat(1, Nat::_signed, 65) >> 1 == Nat({0xffffffff, 0xffffffff,1}, Nat::_signed, 65));
	assert(Nat({0xffffffff, 0x7fffffff,1}, Nat::_signed, 65) >> 1 == Nat({0xffffffff, 0xbfffffff,1}, Nat::_signed, 65));
	assert(Nat({0xffffffff, 0xffffffff}, Nat::_signed, 65) >> 1 == Nat({0xffffffff, 0x7fffffff}, Nat::_signed, 65));
	assert(-Nat(1, Nat::_unsigned, 65) >> 1 == Nat({0xffffffff, 0xffffffff}, Nat::_unsigned, 65));
	assert(-Nat(1, Nat::_unsigned, 65) >> 2 == Nat({0xffffffff, 0x7fffffff}, Nat::_unsigned, 65));
	assert(-Nat(1, Nat::_unsigned, 65) << 1 == Nat({0xfffffffe, 0xffffffff,1}, Nat::_unsigned, 65));
	assert(-Nat(1, Nat::_unsigned, 65) << 2 == Nat({0xfffffffc, 0xffffffff,1}, Nat::_unsigned, 65));

	/* unsigned comparison */
	assert(Nat(-1, Nat::_unsigned, 32) > Nat(1, Nat::_unsigned, 32));
	assert(Nat(0, Nat::_unsigned, 32) < Nat(1, Nat::_unsigned, 32));
	assert(Nat(1, Nat::_unsigned, 32) > Nat(0, Nat::_unsigned, 32));
	assert(Nat(1, Nat::_unsigned, 32) < Nat(-1, Nat::_unsigned, 32));

	/* signed comparison */
	assert(Nat(-1, Nat::_signed, 32) < Nat(1, Nat::_signed, 32));
	assert(Nat(0, Nat::_signed, 32) < Nat(1, Nat::_signed, 32));
	assert(Nat(1, Nat::_signed, 32) > Nat(0, Nat::_signed, 32));
	assert(Nat(1, Nat::_signed, 32) > Nat(-1, Nat::_signed, 32));

	return 0;
}
