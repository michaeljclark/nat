/*
 * nat tests
 *
 * simple test cases for unsigned natural number implementation
 *
 * c++ -O3 -std=c++11 src/nat-tests.cc -o bin/nat-tests
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

#include "nat.h"

int main(int argc, char const *argv[])
{
	/* test empty constructor */
	nat b1;
	assert(b1.num_limbs() == 1);
	assert(b1.limb_at(0) == 0);

	/* test integral constructor */
	nat b2(1);
	assert(b2.num_limbs() == 1);
	assert(b2.limb_at(0) == 1);

	/* test array constructor */
	nat b3{1, 1};
	assert(b3.num_limbs() == 2);
	assert(b3.limb_at(0) == 1);
	assert(b3.limb_at(1) == 1);

	/* test left shift */
	nat b4 = 1;
	b4 = b4 << 1;
	assert(b4.num_limbs() == 1);
	assert(b4.limb_at(0) == 2);

	/* test left shift carry overflow */
	nat b5(std::numeric_limits<nat::limb_t>::max());
	b5 = b5 << 10;
	assert(b5.num_limbs() == 2);
	assert(b5.limb_at(0) == std::numeric_limits<nat::limb_t>::max() - 1023);
	assert(b5.limb_at(1) == 1023);

	/* test big left shift */
	nat b6 = nat{1, 1} << 2;
	assert(b6.num_limbs() == 2);
	assert(b6.limb_at(0) == 4);
	assert(b6.limb_at(1) == 4);

	/* test right shift carry under */
	assert((nat{0,1} >> 1) == nat(1ULL << (nat::limb_bits - 1)));

	/* test right shift carry under limb contraction */
	assert((nat{0,1} >> 1).num_limbs() == 1);

	/* test big right shift */
	assert((nat{0,1} >> 17) == nat(1ULL << (nat::limb_bits - 17)));

	/* test logical and */
	nat b7 = nat{0b101, 0b101} & nat{0b100, 0};
	assert(b7.num_limbs() == 1);
	assert(b7.limb_at(0) == 0b100);

	/* test logical or */
	nat b8 = nat{0b101, 0b101} | nat{0b100, 0};
	assert(b8.num_limbs() == 2);
	assert(b8.limb_at(0) == 0b101);
	assert(b8.limb_at(1) == 0b101);

	/* test equals */
	assert((nat{2,3} == nat{2,3,0}));
	assert((nat{2,3,0} == nat{2,3}));
	assert((nat{1,2,3} == nat{1,2,3}));
	assert(!(nat{1,2,3} == nat{3,2,3}));
	assert(!(nat{1,2,3} == 0));
	assert(!(nat{1,2,3} == 2147483648));

	/* test not equals */
	assert(!(nat{2,3} != nat{2,3,0}));
	assert(!(nat{2,3,0} != nat{2,3}));
	assert(!(nat{1,2,3} != nat{1,2,3}));
	assert((nat{1,2,3} != nat{3,2,3}));
	assert((nat{1,2,3} != 0));
	assert((nat{1,2,3} != 2147483648));

	/* test less than */
	assert(!(nat{3,2,3} < nat{1,2,3}));
	assert(!(nat{1,2,3} < nat{1,2,3}));
	assert((nat{1,2,3} < nat{3,2,3}));
	assert((nat{1,2,3} < nat{3,2,3,1}));
	assert(!(nat{3,2,3,1} < nat{1,2,3}));
	assert(!(nat{3,2,3,1} < 0));
	assert(!(nat{3,2,3,1} < 2147483648));

	/* test less than equals */
	assert(!(nat{3,2,3} <= nat{1,2,3}));
	assert((nat{1,2,3} <= nat{1,2,3}));
	assert((nat{1,2,3} <= nat{3,2,3}));
	assert((nat{1,2,3} <= nat{3,2,3,1}));
	assert(!(nat{3,2,3,1} <= nat{1,2,3}));
	assert(!(nat{3,2,3,1} <= 0));
	assert(!(nat{3,2,3,1} <= 2147483648));

	/* test greater than */
	assert((nat{3,2,3} > nat{1,2,3}));
	assert(!(nat{1,2,3} > nat{1,2,3}));
	assert(!(nat{1,2,3} > nat{3,2,3}));
	assert(!(nat{1,2,3} > nat{3,2,3,1}));
	assert((nat{3,2,3,1} > nat{1,2,3}));
	assert((nat{3,2,3,1} > 0));
	assert((nat{3,2,3,1} > 2147483648));

	/* test greater than equals */
	assert((nat{3,2,3} >= nat{1,2,3}));
	assert((nat{1,2,3} >= nat{1,2,3}));
	assert(!(nat{1,2,3} >= nat{3,2,3}));
	assert(!(nat{1,2,3} >= nat{3,2,3,1}));
	assert((nat{3,2,3,1} >= nat{1,2,3}));
	assert((nat{3,2,3,1} >= 0));
	assert((nat{3,2,3,1} >= 2147483648));

	/* test addition */
	assert((nat{1,1,1} + nat{1,1,1} == nat{2,2,2}));

	/* test addition with carry */
	nat b9{0,std::numeric_limits<nat::limb_t>::max()};
	nat b10{0,std::numeric_limits<nat::limb_t>::max()-1,1};
	assert(b9 + b9 == b10);

	/* test multiplication */
	nat b12 = nat(2147483648) * nat(2147483648);
	assert(b12.num_limbs() == 2);
	assert(b12.limb_at(0) == 0);
	assert(b12.limb_at(1) == 1073741824);
	nat b13 = b12 * b12;
	assert(b13.num_limbs() == 4);
	assert(b13.limb_at(0) == 0);
	assert(b13.limb_at(1) == 0);
	assert(b13.limb_at(2) == 0);
	assert(b13.limb_at(3) == 268435456);
	nat b14 = nat(2147483647) * nat(2147483647);
	assert(b14.num_limbs() == 2);
	assert(b14.limb_at(0) == 1);
	assert(b14.limb_at(1) == 1073741823);
	nat b15 = b14 * b14;
	assert(b15.num_limbs() == 4);
	assert(b15.limb_at(0) == 1);
	assert(b15.limb_at(1) == 2147483646);
	assert(b15.limb_at(2) == -2147483647);
	assert(b15.limb_at(3) == 268435455);

	/* test subtraction */
	assert((nat{3,3,3} - nat{1,1,1} == nat{2,2,2}));

	/* test subtraction with borrow */
	nat b17{1,1};
	nat b18{std::numeric_limits<nat::limb_t>::max()};
	assert(b17-b18 == 2);

	/* test division */
	nat b19 = b15 / b14;
	assert(b19.num_limbs() == 2);
	assert(b19.limb_at(0) == 1);
	assert(b19.limb_at(1) == 1073741823);
	assert(b19.to_string() == "4611686014132420609");

	/* test set and test bit */
	nat b20;
	b20.set_bit(64);
	assert(b20.test_bit(64) == 1);
	assert(b20.test_bit(63) == 0);
	b20 = b20 - 1;
	assert(b20.test_bit(64) == 0);
	assert(b20.test_bit(63) == 1);

	/* string formatting */
	assert((nat(9) * nat(9)).to_string() == "81");
	assert((nat(255) * nat(255)).to_string() == "65025");
	assert((nat(65535) * nat(65535)).to_string() == "4294836225");
	assert((nat(2147483648) * nat(2147483648)).to_string() == "4611686018427387904");

	/* pow */
	assert(nat(71).pow(0) == 1);
	assert(nat(71).pow(1) == 71);
	assert(nat(71).pow(17).to_string() == "29606831241262271996845213307591");

	return 0;
}
