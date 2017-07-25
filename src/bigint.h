/*
 * bigint header-only library
 *
 * Michael Clark <michaeljclark@mac.com> 27/1/2017
 *
 * requires a C++11 compiler
 */

struct bigint
{
	/*! limb type */
	typedef unsigned long long limb_t;

	/*! limb bit width and bit shift */
	enum { limb_bits = 64, limb_shift = 6 };

	/*!
	 * limbs is a vector of words with the little end at offset 0
	 */
	std::vector<limb_t> limbs;

	/*! expand limbs to match operand */
	void expand(const bigint &operand)
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
	bigint() : limbs{ 0 } {}

	/*! integral constructor */
	bigint(limb_t n) : limbs{ n } {}

	/*! array constructor */
	bigint(std::initializer_list<limb_t> l) : limbs(l) {}

	/*! copy constructor  */
	bigint(const bigint &operand) : limbs(operand.limbs) {}


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

	/*! add with carry equals */
	bigint& operator+=(const bigint &operand)
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
	bigint& operator-=(const bigint &operand)
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
		return *this;
	}

	/*! left shift equals */
	bigint& operator<<=(int shamt)
	{
		for (int i = 0; i < shamt; i++) {
			limb_t carry = 0;
			for (size_t j = 0; j < limbs.size(); j++) {
				limb_t old_val = limbs[j];
				limb_t new_val = (old_val << 1) | carry;
				limbs[j] = new_val;
				carry = new_val < old_val;
			}
			if (carry) {
				limbs.push_back(1);
			}
		}
		return *this;
	}

	/*! right shift equals */
	bigint& operator>>=(int shamt)
	{
		for (int i = 0; i < shamt; i++) {
			limb_t carry = 0;
			for (size_t j = limbs.size(); j > 0; j--) {
				limb_t old_val = limbs[j - 1];
				limb_t new_val = (old_val >> 1) | carry;
				limbs[j - 1] = new_val;
				carry = (old_val & 1) << (limb_bits - 1);
			}
		}
		contract();
		return *this;
	}

	/*! logical and equals */
	bigint& operator&=(const bigint &operand)
	{
		expand(operand);
		for (size_t i = 0; i < limbs.size(); i++) {
			limbs[i] = operand.limb_at(i) & limbs[i];
		}
		contract();
		return *this;
	}

	/*! logical or equals */
	bigint& operator|=(const bigint &operand)
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
	bigint operator+(const bigint &operand) const
	{
		bigint result(*this);
		return result += operand;
	}

	/*! subtract with borrow */
	bigint operator-(const bigint &operand) const
	{
		bigint result(*this);
		return result -= operand;
	}

	/*! left shift */
	bigint operator<<(int shamt) const
	{
		bigint result(*this);
		return result <<= shamt;
	}

	/*! right shift */
	bigint operator>>(int shamt) const
	{
		bigint result(*this);
		return result >>= shamt;
	}

	/*! logical and */
	bigint operator&(const bigint &operand) const
	{
		bigint result(*this);
		return result &= operand;
	}

	/*! logical or */
	bigint operator|(const bigint &operand) const
	{
		bigint result(*this);
		return result |= operand;
	}


	/*
	 * comparison are defined in terms of "equals" and "less than"
	 */

	/*! equals */
	bool operator==(const bigint &operand) const
	{
		size_t max = std::max(num_limbs(), operand.num_limbs());
		for (size_t i = 0; i < max; i++) {
			if (limb_at(i) != operand.limb_at(i)) return false;
		}
		return true;
	}

	/*! less than */
	bool operator<(const bigint &operand) const
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
	bool operator!=(const bigint &operand) const { return !(*this == operand); }

	/*! less than or equal*/
	bool operator<=(const bigint &operand) const { return *this < operand || *this == operand; }

	/*! greater than */
	bool operator>(const bigint &operand) const { return !(*this <= operand); }

	/*! less than or equal*/
	bool operator>=(const bigint &operand) const { return *this > operand || *this == operand; }


	/*
	 * implement school book multply, divide, modulus and pow
	 *
	 * These routines uses simple school book binary multiply
	 * and divide. Karatsuba base 2^64 multiply and Newton-Raphson
	 * divide would be faster but time was limited.
	 */

	/*! school book binary multiply */
	bigint operator*(const bigint &operand)
	{
		bigint result(0);
		bigint multiplicand = *this, multiplier = operand;
		while(multiplier > 0) {
			if ((multiplier & 1) == 1) result += multiplicand;
			multiplier >>= 1;
			multiplicand <<= 1;
		}
		result.contract();
		return result;
	}

	/*! school book binary division quotient */
	bigint operator/(const bigint &divisor)
	{
		if (divisor == 0) return 0; /* divide by zero */
		bigint quotient(0), remainder(0);
		for (size_t i = num_limbs() * limb_bits; i > 0; i--) {
			remainder <<= 1;
			remainder |= test_bit(i - 1);
			if (remainder >= divisor) {
				remainder -= divisor;
				quotient.set_bit(i - 1);
			}
		}
		quotient.contract();
		return quotient;
	}

	/*! school book binary division remainder */
	bigint operator%(const bigint &divisor)
	{
		if (divisor == 0) return 0; /* divide by zero */
		bigint remainder(0);
		for (size_t i = num_limbs() * limb_bits; i > 0; i--) {
			remainder <<= 1;
			remainder |= test_bit(i - 1);
			if (remainder >= divisor) {
				remainder -= divisor;
			}
		}
		remainder.contract();
		return remainder;
	}

	/*! raise to the power */
	bigint pow(size_t operand)
	{
		/* 0 to the power of anything is 1 */
		if (operand == 0) return bigint(1);
		bigint result(*this);
		for (size_t i = 1; i < operand; i++) {
			result = result * *this;
		}
		return result;
	}


	/*
	 * convert bigint to string
	 */

	/*! convert big integer to string */
	std::string to_string()
	{
		std::string s;
		bigint val = *this;
		do {
			bigint digit = val % 10;
			s.insert(s.begin(), '0' + char(digit.limbs.front()));
			val = val / 10;
		} while (val != 0);
		return s;
	}
};
