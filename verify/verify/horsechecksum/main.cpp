#include <stdio.h>
#include <time.h>
#include <memory.h>
#include "data.h"
#include "test.h"

enum ETEST{
	eOne = 0,
	eTwo = 10,
};
int main()
{
	 init();

	 //开启注释，即可测试
	 //test_by_row();//测试按行排列的位棋盘无法对马腿用折叠位棋盘
	 //test_by_col();//验证按列排列的位棋盘可以对马腿，
	 //test_by_col_knight_eye();//马眼用位棋盘
	 //test_speed();
	 //test_memcpy();
     test_memcmp();

	 

	 getchar();
	return 0;
}