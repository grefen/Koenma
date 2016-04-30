#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "init.h"
#include "position.h"
#include "movegen.h"

static size_t perft(Position& pos, int depth) {

	if (depth < 0)	 return 0;
	if (depth == 0) return MoveList<LEGAL>(pos).size();
	
	StateInfo st;
	uint64_t cnt = 0;
	CheckInfo ci(pos);

	MoveList<LEGAL> it(pos);

	for (auto i = it.begin(); i != it.end(); ++i)
	{
		pos.do_move(*i, st, pos.gives_check(*i, ci));
		cnt += perft(pos, depth - 1);
		pos.undo_move(*i);
	}
	
	return cnt;
}
int main() {
	
	Bitboards::init();
	
	printf("start perft...\n");
	for (int i = 0; i < 15; ++i)
	{
		std::string StartFen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
		Position pos(StartFen, false, nullptr);

		clock_t s = clock();

		uint64_t cn = perft(pos, (i));

		printf("time: %d\n", clock()-s);
		printf("nodes:%lld\n\n", cn);
		system("pause");
	}
	
	system("pause");
	return 0;
}