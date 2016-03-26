#include<stdlib.h>
#include<Windows.h>
#include <math.h>
#include <time.h>
#include"data.h"
#include"position.h"

uint64_t do_perft(Position& pos, int depth);

int main() {
	printf("imagic bitboard 64bit + evasion use 2.5s for 5 depth perft\n");
	init_data();
	do
	{
		printf("please input perft depth(0~4):\n");

		char d[1024] = { 0 };
		gets_s(d, 1024);
		int depth = atoi(d);


		std::string StartFen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
		Position pos;
		pos.set(StartFen);

		clock_t t = clock();
		uint64_t total = do_perft(pos, depth);

		printf("nodes: %I64d, time:  %d\n\n", total, clock() - t);


	} while (1);


	system("pause");
}
uint64_t do_perft(Position& pos, int depth)
{
	if (depth < 0) return 0;

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
		pos.do_move(list.mlist[i].move, st, ci, pos.gives_check(list.mlist[i].move, ci));
		total += do_perft(pos, depth - 1);
		pos.undo_move(list.mlist[i].move);
	}

	return total;
}