#ifndef BITBOARD_TEST_H
#define BITBOARD_TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <math.h>
#include <time.h>
#include "bitboard.h"
#include "data.h"
#include "position.h"
#include "windows.h"


void test_bitboard(FILE *fp);
void test_initdata(FILE *fp);
void test_position(FILE *fp);
void test_perft();

void test_main(){

	//FILE *fp = fopen("test_biboard.txt","wb");

    //test_bitboard(stdout);
	//test_initdata(fp);
	//test_position(fp);

	test_perft();

	//fclose(fp);

	getchar();
}

void test_bitboard(FILE *fp)
{
	Bitboard board1(-1,-1);

	for (int i = 0; i < 128; ++i) {
		//board1.pop_lsb();
		//if (board1.more_than_one()) printf("more than one\n");
		//if (board1.equal_to_two()) printf("equal to two\n");
		//board1.print(stdout);

		//board1.print(stdout);
		//board1.printall(stdout);

		//getchar();
	}

	for (int i = 0; i < 96; ++i)
	{
		//fprintf(fp, "-----no %d-----\n",i);
		//Bitboard board = board1<<i;
		//Bitboard board = board1>>i;
		//board.print(fp);
		//board.printl90(fp);
		//board.printall(fp);
	}

	{
		//Bitboard board = board1.operator-(1);
		//board.print(fp);
		//board.printl90(fp);
		//board.printall(fp);
	}

	for (int i = 0; i < 96; ++i)
	{
		//fprintf(fp, "-----no %d-----\n",i);
		//board1<<=1;
		//board1>>=1;
		//board1.print(fp);
		//board1.printl90(fp);
		//board1.printall(fp);
	}

	{
		//Bitboard board;
		//Bitboard t = ~board;
		//t.print(fp);
		//t.printall(fp);

		//t = ~t;
		//t.print(fp);
		//t.printall(fp);
	}
}

void test_initdata(FILE *fp)
{
	fprintf(fp,"square color:%d %d\n", square_color(Square(0)),square_color(Square(89)));

	//PawnMask[0].print(fp);
	//PawnMask[1].print(fp);
    //for (Square s = SQ_A0; s <= SQ_I9; ++s)
    {
		//fprintf(fp, "square no %d\n", int(s));

        //PawnAttackFrom[0][s].print(fp);
		//PawnAttackTo[0][s].print(fp);

        //KingAttackTo[s].print(fp);

		//AdvisorAttackTo[s].print(fp);
         //if(BishopAttackTo[s][0])
		    //BishopAttackTo[s][0].print(fp);
		//if(KnightAttackFrom[s][0])
			//KnightAttackFrom[s][0].print(fp);

		//if(KnightAttackTo[s][0])
			//KnightAttackTo[s][0].print(fp);

		//if (RookAttackMask[s])
		{
			//RookAttackMask[s].print(fp);
		}

		//if (RookAttackToR0[s][0])
		//{
		//	RookAttackToR0[s][0].print(fp);
		//}

		//if (RookAttackToRL90[s][0])
		//{
		//	RookAttackToRL90[s][0].print(fp);
		//}

		//if (CannonAttackToR0[s][0])
		{
			//CannonAttackToR0[s][0].print(fp);
		}
    }

	for (int i = 0; i < 256; ++i)
	{
        //fprintf(fp, "square no %d\n", int(i));
		//RookAttackToRL90[SQ_A0][i].print(fp);
	}
}

void test_position(FILE *fp)
{
	//根据公式file_of(sq) * 10 + (9-rank_of(sq))，计算映射表
	//for (Square s = SQ_A0; s <= SQ_I9; ++s)
	//{
	//	Square s90 = square_rotate_l90(s);
	//	fprintf(fp,"%d,",s90);
	//	if ((s+1)%10 == 0)
	//	{
	//		fprintf(fp,"\n");
	//	}
	//}
	
	
	
	std::string StartFen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";

	Position pos;
	pos.set(StartFen);

	printf("%s\n",pos.pretty().c_str());

	MoveList list;
	gen_legal_moves(pos, &list);
	printf("total: %d\n",list.size);
	for (int i = 0; i < list.size; ++i)
	{
		std::string move = move_to_chinese(pos, list.mlist[i].move);
        printf("%s, ",move.c_str());
	}
	printf("\n");

}

uint64_t do_perft(Position& pos, int depth);
void test_perft()
{
	
	do 
	{
		//printf("please input perft depth(0~4):\n");

		char d[1024] = {0};
		gets_s(d, 1024);
        int depth = atoi(d);
		
		
		std::string StartFen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
		Position pos;
		pos.set(StartFen);

		clock_t t = clock();
		//uint32_t start = GetTickCount();
		uint64_t total = do_perft(pos, depth);
		//uint32_t end = GetTickCount();
		//uint32_t cost = end - start;
		//printf("nodes: %I64d, time:  %d\n\n", total, cost);
		printf("nodes: %I64d, time:  %d\n\n",total, clock()-t);

		
	} while (1);

}

uint64_t do_perft(Position& pos, int depth)
{
	if(depth < 0) return 0;

	StateInfo st;
	uint64_t cnt = 0;
	CheckInfo ci(pos);
	
	
	MoveList list;
	//gen_legal_moves(pos, &list);
	gen_legal_moves_with_evasions(pos, &list);

	if (depth == 0)
	{
		return list.size;
	}

	uint64_t total = 0;//list.size;

	for (int i = 0; i < list.size; ++i)
	{
		//undo_t undo[1];

		//pos.do_move(list.mlist[i].move, undo);

  //      total += do_perft(pos, depth-1);

		//pos.undo_move(list.mlist[i].move, undo);

		pos.do_move(list.mlist[i].move, st, ci, pos.gives_check(list.mlist[i].move, ci));
		total += do_perft(pos, depth - 1);
		pos.undo_move(list.mlist[i].move);
	}

	return total;
}

#endif