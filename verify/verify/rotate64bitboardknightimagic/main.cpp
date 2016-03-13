#include <stdlib.h>
#include <stdio.h>
#include "bitboardtest.h"
int main()
{
	printf("rotate bitboard + knight imagic solution 64bit Bitboard V1.00\n");
	printf("the result is: depth 5 use 5.6 seconds\n");
	printf("knight imagic method take more time than knight checksum\n");

	init_data();

	test_main();

	return 0;
}