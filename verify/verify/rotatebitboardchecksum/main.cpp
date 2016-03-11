#include <stdlib.h>
#include <stdio.h>
#include "bitboardtest.h"
int main()
{
	printf("rotate bitboard + knight checksum solution V1.00\n");

	printf("the result is: depth 5 use 7.5 seconds\n");

	init_data();

	test_main();

	return 0;
}