/*
 * bigint tests
 *
 * simple test cases for unsigned bigint implementation
 *
 * c++ -O3 -std=c++11 src/bigint-tests.cc -o bin/bigint-tests
 *
 * Michael Clark <michaeljclark@mac.com> 27/1/2017
 *
 * requires a C++11 compiler
 */

#include <cassert>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <initializer_list>

#include "bigint.h"

int main(int argc, char const *argv[])
{
	/* test empty constructor */
	bigint b1;
	assert(b1.num_limbs() == 1);
	assert(b1.limb_at(0) == 0);

	/* test integral constructor */
	bigint b2(1);
	assert(b2.num_limbs() == 1);
	assert(b2.limb_at(0) == 1);

	/* test array constructor */
	bigint b3{1, 1};
	assert(b3.num_limbs() == 2);
	assert(b3.limb_at(0) == 1);
	assert(b3.limb_at(1) == 1);

	/* test left shift */
	bigint b4 = 1;
	b4 = b4 << 1;
	assert(b4.num_limbs() == 1);
	assert(b4.limb_at(0) == 2);

	/* test left shift carry overflow */
	bigint b5(std::numeric_limits<bigint::limb_t>::max());
	b5 = b5 << 10;
	assert(b5.num_limbs() == 2);
	assert(b5.limb_at(0) == std::numeric_limits<bigint::limb_t>::max() - 1023);
	assert(b5.limb_at(1) == 1023);

	/* test big left shift */
	bigint b6 = bigint{1, 1} << 2;
	assert(b6.num_limbs() == 2);
	assert(b6.limb_at(0) == 4);
	assert(b6.limb_at(1) == 4);

	/* test right shift carry under */
	assert((bigint{0,1} >> 1) == bigint(1ULL << (bigint::limb_bits - 1)));

	/* test right shift carry under limb contraction */
	assert((bigint{0,1} >> 1).num_limbs() == 1);

	/* test big right shift */
	assert((bigint{0,1} >> 33) == bigint(1ULL << (bigint::limb_bits - 33)));

	/* test logical and */
	bigint b7 = bigint{0b101, 0b101} & bigint{0b100, 0};
	assert(b7.num_limbs() == 1);
	assert(b7.limb_at(0) == 0b100);

	/* test logical or */
	bigint b8 = bigint{0b101, 0b101} | bigint{0b100, 0};
	assert(b8.num_limbs() == 2);
	assert(b8.limb_at(0) == 0b101);
	assert(b8.limb_at(1) == 0b101);

	/* test equals */
	assert((bigint{2,3} == bigint{2,3,0}));
	assert((bigint{2,3,0} == bigint{2,3}));
	assert((bigint{1,2,3} == bigint{1,2,3}));
	assert(!(bigint{1,2,3} == bigint{3,2,3}));
	assert(!(bigint{1,2,3} == 0));
	assert(!(bigint{1,2,3} == 2147483648));

	/* test not equals */
	assert(!(bigint{2,3} != bigint{2,3,0}));
	assert(!(bigint{2,3,0} != bigint{2,3}));
	assert(!(bigint{1,2,3} != bigint{1,2,3}));
	assert((bigint{1,2,3} != bigint{3,2,3}));
	assert((bigint{1,2,3} != 0));
	assert((bigint{1,2,3} != 2147483648));

	/* test less than */
	assert(!(bigint{3,2,3} < bigint{1,2,3}));
	assert(!(bigint{1,2,3} < bigint{1,2,3}));
	assert((bigint{1,2,3} < bigint{3,2,3}));
	assert((bigint{1,2,3} < bigint{3,2,3,1}));
	assert(!(bigint{3,2,3,1} < bigint{1,2,3}));
	assert(!(bigint{3,2,3,1} < 0));
	assert(!(bigint{3,2,3,1} < 2147483648));

	/* test less than equals */
	assert(!(bigint{3,2,3} <= bigint{1,2,3}));
	assert((bigint{1,2,3} <= bigint{1,2,3}));
	assert((bigint{1,2,3} <= bigint{3,2,3}));
	assert((bigint{1,2,3} <= bigint{3,2,3,1}));
	assert(!(bigint{3,2,3,1} <= bigint{1,2,3}));
	assert(!(bigint{3,2,3,1} <= 0));
	assert(!(bigint{3,2,3,1} <= 2147483648));

	/* test greater than */
	assert((bigint{3,2,3} > bigint{1,2,3}));
	assert(!(bigint{1,2,3} > bigint{1,2,3}));
	assert(!(bigint{1,2,3} > bigint{3,2,3}));
	assert(!(bigint{1,2,3} > bigint{3,2,3,1}));
	assert((bigint{3,2,3,1} > bigint{1,2,3}));
	assert((bigint{3,2,3,1} > 0));
	assert((bigint{3,2,3,1} > 2147483648));

	/* test greater than equals */
	assert((bigint{3,2,3} >= bigint{1,2,3}));
	assert((bigint{1,2,3} >= bigint{1,2,3}));
	assert(!(bigint{1,2,3} >= bigint{3,2,3}));
	assert(!(bigint{1,2,3} >= bigint{3,2,3,1}));
	assert((bigint{3,2,3,1} >= bigint{1,2,3}));
	assert((bigint{3,2,3,1} >= 0));
	assert((bigint{3,2,3,1} >= 2147483648));

	/* test addition */
	assert((bigint{1,1,1} + bigint{1,1,1} == bigint{2,2,2}));

	/* test addition with carry */
	bigint b9{0,std::numeric_limits<bigint::limb_t>::max()};
	bigint b10{0,std::numeric_limits<bigint::limb_t>::max()-1,1};
	assert(b9 + b9 == b10);

	/* test multiplication */
	bigint b12 = bigint(2147483648) * bigint(2147483648);
	assert(b12.num_limbs() == 1);
	assert(b12.limb_at(0) == 4611686018427387904ULL);
	bigint b13 = b12 * b12;
	assert(b13.num_limbs() == 2);
	assert(b13.limb_at(0) == 0);
	assert(b13.limb_at(1) == 1152921504606846976ULL);
	bigint b14 = bigint(2147483647) * bigint(2147483647);
	assert(b14.num_limbs() == 1);
	assert(b14.limb_at(0) == 4611686014132420609ULL);
	bigint b15 = b14 * b14;
	assert(b15.num_limbs() == 2);
	assert(b15.limb_at(0) == 9223372028264841217ULL);
	assert(b15.limb_at(1) == 1152921502459363329ULL);
	bigint b16 = b15 * b15;
	assert(b16.num_limbs() == 4);
	assert(b16.limb_at(0) == 18446744056529682433ULL);
	assert(b16.limb_at(1) == 6917528997576310790ULL);
	assert(b16.limb_at(2) == 8070450524731736068ULL);
	assert(b16.limb_at(3) == 72057593769492480ULL);

	/* test subtraction */
	assert((bigint{3,3,3} - bigint{1,1,1} == bigint{2,2,2}));

	/* test subtraction with borrow */
	bigint b17{1,1};
	bigint b18{std::numeric_limits<bigint::limb_t>::max()};
	assert(b17-b18 == 2);

	/* test division */
	bigint b19 = b15 / b14;
	assert(b19.num_limbs() == 1);
	assert(b19.limb_at(0) == 4611686014132420609ULL);

	/* test set and test bit */
	bigint b20;
	b20.set_bit(64);
	assert(b20.test_bit(64) == 1);
	assert(b20.test_bit(63) == 0);
	b20 = b20 - 1;
	assert(b20.test_bit(64) == 0);
	assert(b20.test_bit(63) == 1);
	assert(b20 == 18446744073709551615ULL);

	/* string formatting */
	assert((bigint(9) * bigint(9)).to_string() == "81");
	assert((bigint(255) * bigint(255)).to_string() == "65025");
	assert((bigint(65535) * bigint(65535)).to_string() == "4294836225");
	assert((bigint(2147483648) * bigint(2147483648)).to_string() == "4611686018427387904");
	assert((bigint(4611686014132420609ULL) * bigint(4611686014132420609ULL)).to_string() == "21267647892944572736998860269687930881");

	/* pow */
	assert(bigint(71).pow(0) == 1);
	assert(bigint(71).pow(1) == 71);
	assert(bigint(71).pow(17).to_string() == "29606831241262271996845213307591");

	return 0;
}
