// rookandcannonimagicgen.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include "data.h"

int _tmain(int argc, _TCHAR* argv[])
{
	printf("this program aim to use imagic bitboard\n");
	printf("can not find imagic for SQ_A3 and SQ_G3 32bit Bitboard\n");
	
	init_data();
	
	system("pause");
	return 0;
}

