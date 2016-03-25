#include <stdlib.h>
#include <stdio.h>
#include "bitboardtest.h"
int main()
{
	printf("rotate bitboard + knight checksum solution 64bit Bitboard V1.00\n");

	printf("use king evasions, about 2.88s for 5 depth perft\n");

	//setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	init_data();

	test_main();

	return 0;
}