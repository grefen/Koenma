#ifndef MOVEGEN_H
#define MOVEGEN_H
#include "types.h"

class Position;

enum Move {
	MOVE_NONE,
	MOVE_NULL = 0xffff
};

ENABLE_OPERATORS_ON(Move)

struct ExtMove {
	Move move;
	int score;
};

struct MoveList {
	MoveList(){ size = 0; };

    uint32_t size;
	ExtMove mlist[256];
};

inline Square from_sq(Move m) {
	return Square((m >> 8));
}

inline Square to_sq(Move m) {
	return Square(m & 0xFF);
}

inline Move make_move(Square from, Square to) {
	return Move(to | (from << 8));
}

inline bool is_ok(Move m) {
	return from_sq(m) != to_sq(m); // Catches also MOVE_NULL and MOVE_NONE
}



extern std::string move_to_chinese(const Position& pos, Move m);
extern void gen_legal_moves(Position& pos, MoveList* mlist);

#endif