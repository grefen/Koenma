#ifndef test_h
#define test_h
#include <time.h>
#include <stdlib.h>
#include "checksum.h"
#include "data.h"

//证明sq按照行排列无法使马腿checksum避免冲突
void test_by_row()
{
	//只要验证，某个位置，马腿的所有可能情形，相互之间不会产生hash冲突即可
	printf("--------test_by_row------------\n");
	int HorseLegsStep[4][2] = {{ 0,1},{0,-1},{ -1,0},{1,0}};

	Bitboard knightleg(0,0,0);
	Bitboard arrBoard[90][16];

	//针对一个位置
	for(int sq = 0; sq < 1; ++sq)
	{
	
		int bdcount = 0;
		//遍历每种情形
		for (int t = 0; t <= 1; t++)
		{
			for(int b = 0; b <= 1; b++)
			{
				for (int l = 0; l <= 1; l++)
				{
					for (int r = 0; r <= 1; r++)
					{    //上下左右的各种组合，目的是穷举


						knightleg = Bitboard(0,0,0);
						//马脚位棋盘
						if(t)
						{
							int r = SqToRankEx[sq] + HorseLegsStep[0][1];//行
							int f = SqToFileEx[sq] + HorseLegsStep[0][0];//列
							if((r>= 0&& r<=9) && f>=0 && f<=8)
							{
								knightleg |=BIT((r*9+f)) ;
							}
						}
						if (b)
						{
							int r = SqToRankEx[sq] + HorseLegsStep[1][1];//行
							int f = SqToFileEx[sq] + HorseLegsStep[1][0];//列
							if((r>= 0&& r<=9) && f>=0 && f<=8)
							{
								knightleg |=BIT((r*9+f)) ;
							}
						}
						if (l)
						{
							int r = SqToRankEx[sq] + HorseLegsStep[2][1];//行
							int f = SqToFileEx[sq] + HorseLegsStep[2][0];//列
							if((r>= 0&& r<=9) && f>=0 && f<=8)
							{
								knightleg |=BIT((r*9+f)) ;
							}
						}
						if (r)
						{
							int r = SqToRankEx[sq] + HorseLegsStep[3][1];//行
							int f = SqToFileEx[sq] + HorseLegsStep[3][0];//列
							if((r>= 0&& r<=9) && f>=0 && f<=8)
							{
								knightleg |=BIT((r*9+f)) ;
							}
						}

						arrBoard[sq][bdcount++] = knightleg;	
						
					}
				}
			}
		}

	}

	for (int sq = 0; sq < 1; ++sq)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = i+1;j < 16; j++ )
			{
				if (arrBoard[sq][i] != arrBoard[sq][j])
				{
					if (checksum(arrBoard[sq][i]) == checksum(arrBoard[sq][j]))
					{
						printf("conflit sq %d\n",sq);
                        arrBoard[sq][i].print();
                        arrBoard[sq][j].print();
                        printf("check %d\n", checksum(arrBoard[sq][i]));
					}
				}
			}
		}
	}


	printf("------这种情况有冲突，所以不能按照行排列--------------\n");
	getchar();
}
//证明sq按照列排列可以使马腿checksum避免冲突
void test_by_col()
{
	//只要验证，某个位置，马腿的所有可能情形，相互之间不会产生hash冲突即可
	printf("--------test_by_col------------\n");
	int HorseLegsStep[4][2] = {{ 0,1},{0,-1},{ -1,0},{1,0}};

	Bitboard knightleg(0,0,0);
	Bitboard arrBoard[90][16];

	//针对一个位置
	for(int sq = 0; sq < 90; ++sq)
	{

		int bdcount = 0;
		//遍历每种情形
		for (int t = 0; t <= 1; t++)
		{
			for(int b = 0; b <= 1; b++)
			{
				for (int l = 0; l <= 1; l++)
				{
					for (int r = 0; r <= 1; r++)
					{    //上下左右的各种组合，目的是穷举


						knightleg = Bitboard(0,0,0);
						//马脚位棋盘
						if(t)
						{
							int r = SqToRank[sq] + HorseLegsStep[0][1];//行
							int f = SqToFile[sq] + HorseLegsStep[0][0];//列
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}
						if (b)
						{
							int r = SqToRank[sq] + HorseLegsStep[1][1];//行
							int f = SqToFile[sq] + HorseLegsStep[1][0];//列
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}
						if (l)
						{
							int r = SqToRank[sq] + HorseLegsStep[2][1];//行
							int f = SqToFile[sq] + HorseLegsStep[2][0];//列
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}
						if (r)
						{
							int r = SqToRank[sq] + HorseLegsStep[3][1];//行
							int f = SqToFile[sq] + HorseLegsStep[3][0];//列
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}

						arrBoard[sq][bdcount++] = knightleg;

						//knightleg.print();

					}
				}
			}
		}

	}

	for (int sq = 0; sq < 90; ++sq)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = i+1;j < 16; j++ )
			{
				if (arrBoard[sq][i] != arrBoard[sq][j])
				{
					if (checksum(arrBoard[sq][i]) == checksum(arrBoard[sq][j]))
					{
						printf("conflit sq %d\n",sq);
						arrBoard[sq][i].print();
						arrBoard[sq][j].print();
						printf("check %d\n", checksum(arrBoard[sq][i]));
					}
				}
			}
		}
	}


	printf("-------证明了要使马腿的checksum不冲突，必须按照列来排列sq-------------\n");
	getchar();
}

//证明上面按照列排列的情况同样适用于马眼
void test_by_col_knight_eye()
{
	//只要验证，某个位置，马眼的所有可能情形，相互之间不会产生hash冲突即可
	printf("--------test_by_col_knight_eye------------\n");
	int HorseEyeStep[4][2] = {{ 1,1},{-1,-1},{ -1,1},{1,-1}};

	Bitboard knightleg(0,0,0);
	Bitboard arrBoard[90][16];

	//针对一个位置
	for(int sq = 0; sq < 90; ++sq)
	{

		int bdcount = 0;
		//遍历每种情形
		for (int t = 0; t <= 1; t++)
		{
			for(int b = 0; b <= 1; b++)
			{
				for (int l = 0; l <= 1; l++)
				{
					for (int r = 0; r <= 1; r++)
					{    //上下左右的各种组合，目的是穷举


						knightleg = Bitboard(0,0,0);
						//马脚位棋盘
						if(t)
						{
							int r = SqToRank[sq] + HorseEyeStep[0][1];//行
							int f = SqToFile[sq] + HorseEyeStep[0][0];//列
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}
						if (b)
						{
							int r = SqToRank[sq] + HorseEyeStep[1][1];//行
							int f = SqToFile[sq] + HorseEyeStep[1][0];//列
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}
						if (l)
						{
							int r = SqToRank[sq] + HorseEyeStep[2][1];//行
							int f = SqToFile[sq] + HorseEyeStep[2][0];//列
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}
						if (r)
						{
							int r = SqToRank[sq] + HorseEyeStep[3][1];//行
							int f = SqToFile[sq] + HorseEyeStep[3][0];//列
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}

						arrBoard[sq][bdcount++] = knightleg;

						//knightleg.print();

					}
				}
			}
		}

	}

	for (int sq = 0; sq < 90; ++sq)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = i+1;j < 16; j++ )
			{
				if (arrBoard[sq][i] != arrBoard[sq][j])
				{
					if (checksum(arrBoard[sq][i]) == checksum(arrBoard[sq][j]))
					{
						printf("conflit sq %d\n",sq);
						arrBoard[sq][i].print();
						arrBoard[sq][j].print();
						printf("check %d\n", checksum(arrBoard[sq][i]));
					}
				}
			}
		}
	}


	printf("-------证明了要使马眼的checksum不冲突，必须按照列来排列sq-------------\n");
	getchar();
}

//test_by_col_knight_eye同样也证明了相眼的checksum也不冲突

void test_speed()
{
	//测试checksum速度与提取马腿mask组成的hashkey哪个更快

	//计算checksum
	__int64 count = 99999999999;

	clock_t start = clock(); 
	for(__int64 i = 0; i < count; ++i)
	{
		checksum(KnightLegs[45]);
	}

	printf("%d\n",clock()-start);


    int HorseLegsStep[4][2] = {{ 0,1},{0,-1},{ -1,0},{1,0}};

	int k = 0;
	int s = 45;
	Bitboard board(0,0,0);
    start = clock();
	for(__int64 k = 0; k < count; ++k)
	{
		
		int b = 0;
		for (int i = 0; i < 4; ++i)
		{

			int r = SqToRank[s] + HorseLegsStep[0][1];//行
			int f = SqToFile[s] + HorseLegsStep[0][0];//列

			b |= (!!(BIT(SQ(r,f))&board))<<i;
			
		}
	}

	printf("%d\n",clock()-start);

	//在releaase模式下，两种方法速度上并无差异
	//明显第二中方法执行的指令多，可能编译器做了优化；
	getchar();

}
#endif