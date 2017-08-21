#include <stdio.h>
#include <gmp.h>

int main()
{
	int val = 71, power = 71117;
    mpz_t result, base;
    mpz_init_set_ui(base, val);
    mpz_pow_ui(result, base, power);
    printf("%d^%d is %s\n", val, power, mpz_get_str (NULL, 10, result));
}
