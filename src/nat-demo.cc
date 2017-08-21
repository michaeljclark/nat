#include <cstdio>
#include "nat.h"

int main(int argc, char const *argv[])
{
	int val = 71, power = 71117;
	Nat result =  Nat(val).pow(power);
	printf("%d^%d is %s\n", val, power, result.to_string().c_str());
}
