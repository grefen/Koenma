#include "movegen.h"
#include "position.h"
#include "data.h"

#define SERIALIZE(b) while(b) mlist->mlist[mlist->size++].move = make_move(from, pop_lsb(&b))


static const char* PieceMap[COLOR_NB] = { " PBANCRK", " pbancrk" };

static std::string piece_to_chinese(char p)
{
	switch(p)
	{
	case ' ': return std::string(" ");
	case 'P': return std::string("��");
	case 'p': return std::string("��");
	case 'B': return std::string("��");
	case 'b': return std::string("��");
	case 'A': return std::string("ʿ");
	case 'a': return std::string("��");
	case 'N': return std::string("��");
	case 'n': return std::string("�R");
	case 'C': return std::string("��");
	case 'c': return std::string("��");
	case 'R': return std::string("��");
	case 'r': return std::string("܇");
	case 'K': return std::string("˧");
	case 'k': return std::string("��");
	}

	return std::string();
}

std::string move_to_chinese(const Position& pos, Move m)
{
	Color us = pos.side_to_move();
	Square from = from_sq(m);
	Square to = to_sq(m);
	Piece pc = pos.piece_on(from);
	PieceType pt = type_of(pc);

	char p = PieceMap[us][pt];

	std::string move = "\n";

	move += piece_to_chinese(p);
	move += square_to_string(from);
	move += square_to_string(to);

	return move;
}

static bool move_is_legal(const Position& pos, ExtMove move)
{
	Move m = move.move;
	assert(is_ok(m));	

	Color us = pos.side_to_move();
	Square from = from_sq(m);
	Square to   = to_sq(m);

	assert(color_of(pos.piece_moved(m)) == us);
	assert(pos.piece_on(pos.king_square(us)) == make_piece(us, KING));

	PieceType pt = type_of(pos.piece_on(from));

	Bitboard pawns   = pos.pieces(~us, PAWN);
	Bitboard knights = pos.pieces(~us, KNIGHT);
	Bitboard cannons = pos.pieces(~us, CANNON);
	Bitboard rooks = pos.pieces(~us, ROOK);
	if(pawns & to)
	{
		pawns ^= to;
	}
	else if(knights & to)
	{

		knights ^= to;
	}
	else if(cannons & to)
	{
		cannons ^= to;
	}
	else if(rooks & to)
	{

		rooks ^= to;
	}

	Bitboard  occ    = pos.pieces();
	Bitboard  occl90 = pos.piecesl90();

	occl90 ^= square_rotate_l90(from);
	occ    ^= from;

	if(type_of(pos.piece_on(to)) == NO_PIECE_TYPE)
	{
		occl90 ^= square_rotate_l90(to);
		occ    ^= to;
	}

	Square ksq = pos.king_square(us);
	if(ksq == from)
		ksq = to;

	if((cannon_control_bb(ksq, occ,occl90) & cannons)) return false;
	if((rook_attacks_bb(ksq,occ,occl90)& rooks) ) return false;
	if((knight_attacks_from(ksq, occ,occl90)&knights) ) return false;
	if((pawn_attacks_from(~us,ksq) & pawns) ) return false;

	if((rook_attacks_bb(ksq,occ,occl90)& pos.king_square(~us))) return false;//����

	return true;
}

static void gen_rook_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<ROOK>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		Bitboard att = pos.attacks_from<ROOK>(from)&target;

		SERIALIZE(att);
	}
}
static void gen_knight_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<KNIGHT>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		Bitboard att = pos.attacks_from<KNIGHT>(from)&target;

		SERIALIZE(att);
	}
}
static void gen_cannon_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = pos.pieces(~us);
	Bitboard empty  = ~pos.pieces();
	const Square* pl = pos.list<CANNON>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		Bitboard att = pos.attacks_from<CANNON>(from)&target;

		SERIALIZE(att);

		Bitboard natt = pos.attacks_from<ROOK>(from)&empty;
		SERIALIZE(natt);
	}
}
static void gen_pawn_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = ~pos.pieces(us);
	Bitboard pawns  = pos.pieces(us, PAWN);

	const Square   Up       = (us == WHITE ? DELTA_N  : DELTA_S);
	const Square   Right    = (us == WHITE ? DELTA_E : DELTA_W);
	const Square   Left     = (us == WHITE ? DELTA_W : DELTA_E);

	const Bitboard MaskBB   =  PawnMask[us];

	Bitboard attup = shift_bb(pawns,Up) & MaskBB & target;
	Bitboard attleft = shift_bb(pawns,Left) & MaskBB & target;
	Bitboard attright= shift_bb(pawns,Right) & MaskBB & target;

	while(attup){
		Square to = pop_lsb(&attup);
		Square from = to - (Up);		
		mlist->mlist[mlist->size++].move = make_move(from, to);
	}

	while(attleft){
		Square to = pop_lsb(&attleft);
		Square from = to - (Left);		
		mlist->mlist[mlist->size++].move = make_move(from, to);
	}

	while(attright){
		Square to = pop_lsb(&attright);
		Square from = to - (Right);		
		mlist->mlist[mlist->size++].move = make_move(from, to);
	}
}
static void gen_bishop_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<BISHOP>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		Bitboard att = pos.attacks_from<BISHOP>(from)&target;

		SERIALIZE(att);
	}
}
static void gen_advisor_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<ADVISOR>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		Bitboard att = pos.attacks_from<ADVISOR>(from)&target;

		SERIALIZE(att);
	}
}
static void gen_king_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<KING>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		Bitboard att = pos.attacks_from<KING>(from)&target;

		SERIALIZE(att);
	}
}

void gen_legal_moves(const Position& pos, MoveList* mlist)
{
	gen_rook_moves(pos, mlist);
	gen_knight_moves(pos, mlist);
	gen_cannon_moves(pos, mlist);
	gen_pawn_moves(pos, mlist);
	gen_bishop_moves(pos, mlist);
	gen_advisor_moves(pos, mlist);
	gen_king_moves(pos, mlist);

	uint32_t n = mlist->size;
	for (uint32_t i = 0; i < n; )
	{
		if (!move_is_legal(pos, mlist->mlist[i]))
		{
			mlist->mlist[i].move = mlist->mlist[--n].move;
		}
		else
		{
			++i;
		}
	}
	mlist->size = n;
}