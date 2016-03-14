#include <stdlib.h>
#include <stdio.h>
#include "bitboardtest.h"
int main()
{
	printf("rotate bitboard + knight checksum solution 64bit Bitboard V1.00\n");

	printf("the result is: depth 5 use 5.3 seconds\n");

	init_data();

	test_main();

	return 0;
}