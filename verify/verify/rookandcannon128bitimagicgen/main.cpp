// rookandcannonimagicgen.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include <Windows.h>
#include "data.h"

int main()
{
	printf("64bit Bitboard is ok\n");
	printf("can find all imagic!\n");
	printf("in magic_index <<19 is key\n");	

	printf("rook need %d byte == %dm memory\n", 1081344*16, (1081344*16)/1024/1024);
	
	//gen imagic 
	init_data();
	
	system("pause");
	return 0;
}

