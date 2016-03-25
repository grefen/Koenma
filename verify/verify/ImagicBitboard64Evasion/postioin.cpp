#include "position.h"
#include <cassert>
#include <cstddef>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "movegen.h"

using std::string;
using std::cout;
using std::endl;

static const string PieceToChar(" PBANCRK pbancrk");

/// CheckInfo c'tor
//总结下思路
//总体上采用分层思考方式，把复杂问题简单化
//这里采用checkinfo这个类的目的，就是在genmove之前把当前棋盘
//情况分析一下，主要包括：
//1 我方被牵制的子 2 直接将军的子 3 间接将军的子
//这样的好处是：在genmove的时候可以直接排除掉很多无效招法
//
CheckInfo::CheckInfo(const Position& pos) {

	Color them = ~pos.side_to_move();
	ksq = pos.king_square(them);

	pinned = pos.pinned_pieces(pos.side_to_move());
	dcCandidates = pos.discovered_check_candidates();
	dcCannonCandidates = pos.discovered_cannon_check_candidates();

	//我方将军位置
	checkSq[PAWN] = pawn_attacks_from(~them, ksq);
	checkSq[KNIGHT] = knight_attacks_from(ksq, pos.pieces());
	checkSq[ROOK] = pos.attacks_from<ROOK>(ksq);

	//cannon不能够把所有情况找到,
	//有炮架，通过 dcCannonCandidates
	//cannon 与 king在同一线上，cannon翻打会出现将军的情况(最为复杂)
	checkSq[CANNON] = pos.attacks_from<CANNON>(ksq);

	checkSq[KING] = Bitboard();

	checkSq[BISHOP] = Bitboard();
	checkSq[ADVISOR] = Bitboard();

}

void Position::clear() {

	std::memset(this, 0, sizeof(Position));

	st = &startState;

	for (int i = 0; i < PIECE_TYPE_NB; ++i)
		for (int j = 0; j < 16; j++)
			pieceList[WHITE][i][j] = pieceList[BLACK][i][j] = SQ_NONE;
}

void Position::set(const string& fenStr) {
	/*
	A FEN string defines a particular position using only the ASCII character set.

	A FEN string contains six fields separated by a space. The fields are:

	1) Piece placement (from white's perspective). Each rank is described, starting
	with rank 8 and ending with rank 1; within each rank, the contents of each
	square are described from file A through file H. Following the Standard
	Algebraic Notation (SAN), each piece is identified by a single letter taken
	from the standard English names. White pieces are designated using upper-case
	letters ("PNBRQK") while Black take lowercase ("pnbrqk"). Blank squares are
	noted using digits 1 through 8 (the number of blank squares), and "/"
	separates ranks.

	2) Active color. "w" means white moves next, "b" means black.

	3) Castling availability. If neither side can castle, this is "-". Otherwise,
	this has one or more letters: "K" (White can castle kingside), "Q" (White
	can castle queenside), "k" (Black can castle kingside), and/or "q" (Black
	can castle queenside).

	4) En passant target square (in algebraic notation). If there's no en passant
	target square, this is "-". If a pawn has just made a 2-square move, this
	is the position "behind" the pawn. This is recorded regardless of whether
	there is a pawn in position to make an en passant capture.

	5) Halfmove clock. This is the number of halfmoves since the last pawn advance
	or capture. This is used to determine if a draw can be claimed under the
	fifty-move rule.

	6) Fullmove number. The number of the full move. It starts at 1, and is
	incremented after Black's move.
	*/

	char col, row, token;
	size_t p;
	Square sq = SQ_A9;//last rank,from left to right
	int rule50;
	std::istringstream ss(fenStr);

	clear();
	ss >> std::noskipws;

	// 1. Piece placement
	while ((ss >> token) && !isspace(token))
	{
		if (isdigit(token))
			sq += Square(token - '0'); // Advance the given number of files

		else if (token == '/')
			sq -= Square(18);//9+9

		else if ((p = PieceToChar.find(token)) != string::npos)
		{
			put_piece(sq, color_of(Piece(p)), type_of(Piece(p)));
			++sq;
		}
	}

	// 2. Active color
	ss >> token;
	sideToMove = (token == 'w' ? WHITE : BLACK);
	ss >> token;

	// 3. Castling availability. Compatible with 3 standards: Normal FEN standard,
	// Shredder-FEN that uses the letters of the columns on which the rooks began
	// the game instead of KQkq and also X-FEN standard that, in case of Chess960,
	// if an inner rook is associated with the castling right, the castling tag is
	// replaced by the file letter of the involved rook, as for the Shredder-FEN.
	while ((ss >> token) && !isspace(token))
	{

	}

	// 4. En passant square. Ignore if no pawn capture is possible
	if (((ss >> col) && (col >= 'a' && col <= 'h'))
		&& ((ss >> row) && (row == '3' || row == '6')))
	{

	}

	// 5-6. Halfmove clock and fullmove number
	ss >> std::skipws >> rule50 >> gamePly;

	// Convert from fullmove starting from 1 to ply starting from 0,
	// handle also common incorrect FEN with fullmove = 0.
	gamePly = std::max(2 * (gamePly - 1), 0) + int(sideToMove == BLACK);

	st->checkersBB = checkers(~sideToMove, king_square(sideToMove));// attackers_to(king_square(sideToMove)) & pieces(~sideToMove);
	assert(pos_is_ok());
}


/// Position::fen() returns a FEN representation of the position. In case
/// of Chess960 the Shredder-FEN notation is used. Mainly a debugging function.

const string Position::fen() const {

	std::ostringstream ss;

	for (Rank rank = RANK_9; rank >= RANK_0; --rank)
	{
		for (File file = FILE_A; file <= FILE_I; ++file)
		{
			Square sq = file | rank;

			if (is_empty(sq))
			{
				int emptyCnt = 1;

				for (; file < FILE_I && is_empty(++sq); ++file)
					emptyCnt++;

				ss << emptyCnt;
			}
			else
				ss << PieceToChar[piece_on(sq)];
		}

		if (rank > RANK_0)
			ss << '/';
	}

	ss << (sideToMove == WHITE ? " w " : " b ");

	//ss << (ep_square() == SQ_NONE ? " - " : " " + square_to_string(ep_square()) + " ")
	//    << st->rule50 << " " << 1 + (gamePly - int(sideToMove == BLACK)) / 2;
	ss << "-";
	ss << " ";
	ss << "-";
	ss << " ";
	ss << 0 << " " << 1 + (gamePly - int(sideToMove == BLACK)) / 2;

	return ss.str();
}


/// Position::pretty() returns an ASCII representation of the position to be
/// printed to the standard output together with the move's san notation.

const string Position::pretty(Move move) const {

	string  boards = string("+---+---+---+---+---+---+---+---+\n")
		+ string("|   |   |   |   |   |   |   |   |\n")
		+ string("+---+---+---+---*---+---+---+---+\n")
		+ string("|   |   |   |   |   |   |   |   |\n")
		+ string("+---+---+---+---+---+---+---+---+\n")
		+ string("|   |   |   |   |   |   |   |   |\n")
		+ string("*---+---*---+---*---+---*---+---*\n")
		+ string("|   |   |   |   |   |   |   |   |\n")
		+ string("+---+---+---+---+---+---+---+---+\n")
		+ string("|   |   |   |   |   |   |   |   |\n")
		+ string("+---+---+---+---+---+---+---+---+\n")
		+ string("|   |   |   |   |   |   |   |   |\n")
		+ string("*---+---*---+---*---+---*---+---*\n")
		+ string("|   |   |   |   |   |   |   |   |\n")
		+ string("+---+---+---+---+---+---+---+---+\n")
		+ string("|   |   |   |   |   |   |   |   |\n")
		+ string("+---+---+---+---*---+---+---+---+\n")
		+ string("|   |   |   |   |   |   |   |   |\n")
		+ string("+---+---+---+---+---+---+---+---+\n");


	string brd = boards;

	for (Bitboard b = pieces(); b; )
	{
		Square s = pop_lsb(&b);

		int i = 646 - 34 * (rank_of(s) * 2) - 34 + 4 * file_of(s);
		int c = PieceToChar[piece_on(s)];

		brd[i] = c;
	}

	std::ostringstream ss;
	ss << brd << "\nFen: " << fen();

	//if (move)
	//    ss << "\nMove: " << (sideToMove == BLACK ? ".." : "")
	// << move_to_san(*const_cast<Position*>(this), move)<<std::endl;

	//ss << brd << "\nFen: " << fen() << "\nKey: " << std::hex << std::uppercase
	//	<< std::setfill('0') << std::setw(16) << st->key << "\nCheckers: ";

	//for (Bitboard b = checkers(); b; )
	//	ss << square_to_string(pop_lsb(&b)) << " ";

	//ss << "\nLegal moves: ";
	//for (MoveList<LEGAL> it(*this); *it; ++it)
	//	ss << move_to_san(*const_cast<Position*>(this), *it) << " ";

	return ss.str();
}

bool Position::pos_is_ok(int* failedStep) const
{
	return true;
}

bool Position::in_check(Color c)
{
	Square king = king_square(c);
	Color  opp = ~c;

	Bitboard rooks = pieces(opp, ROOK);
	Bitboard knights = pieces(opp, KNIGHT);
	Bitboard cannons = pieces(opp, CANNON);
	Bitboard pawns = pieces(opp, PAWN);

	if ((rook_attacks_bb(king, occupied) & rooks)) return true;
	if ((knight_attacks_from(king, occupied) & knights)) return true;
	if ((cannon_control_bb(king, occupied) & cannons)) return true;
	if ((pawn_attacks_from(opp, king) & pawns)) return true;
	if ((rook_attacks_bb(king, occupied) & king_square(opp))) return true;

	return false;
}

/// Position::check_blockers() returns a bitboard of all the pieces with color
/// 'c' that are blocking check on the king with color 'kingColor'. A piece
/// blocks a check if removing that piece from the board would result in a
/// position where the king is in check. A check blocking piece can be either a
/// pinned or a discovered check piece, according if its color 'c' is the same
/// or the opposite of 'kingColor'.
//对于cannon，上面所说的 discovered check piece不全部成立
//有漏掉的情况，比较某个字充当炮架
Bitboard Position::check_blockers(Color c, Color kingColor) const {

	Bitboard b, pinners, result;
	Square ksq = king_square(kingColor);

	// Pinners are sliders that give check when a pinned piece is removed

	//rook
	pinners = pieces(~kingColor, ROOK) & RookAttackMask[ksq];
	while (pinners) {
		b = between_bb(ksq, pop_lsb(&pinners)) & pieces();

		if (!b.more_than_one())
			result |= b & pieces(c);
	}

	//cannon
	pinners = pieces(~kingColor, CANNON) & RookAttackMask[ksq];
	while (pinners) {
		b = between_bb(ksq, pop_lsb(&pinners)) & pieces();

		if (b.equal_to_two())
			result |= b & pieces(c);
	}

	//knight
	pinners = pieces(~kingColor, KNIGHT) & KnightAttackMask[ksq];
	while (pinners) {
		//pinner马脚与ksq马眼的交集正好是蹩腿的子
		b = (KnightLeg[pop_lsb(&pinners)] & KnightEye[ksq])& pieces();
		result |= b & pieces(c);
	}

	//king, face to face
	pinners = pieces(~kingColor, KING) & RookAttackMask[ksq];
	while (pinners) {
		b = between_bb(ksq, pop_lsb(&pinners)) & pieces();

		if (!b.more_than_one())
			result |= b & pieces(c);
	}

	return result;
}

Bitboard Position::discovered_check_candidates() const
{
	Bitboard b, pinners, result;
	Color us = side_to_move();
	Color them = ~us;
	Square ksq = king_square(them);


	//潜在的checker位置需要考虑king,cannon,knight

	// Pinners are sliders that give check when a pinned piece is removed
	//rook
	pinners = pieces(us, ROOK) & RookAttackMask[ksq];
	while (pinners) {
		b = between_bb(ksq, pop_lsb(&pinners)) & pieces();

		if (!b.more_than_one())
			result |= b & pieces(us);
	}

	//cannon
	pinners = pieces(us, CANNON) & RookAttackMask[ksq];
	while (pinners) {
		b = between_bb(ksq, pop_lsb(&pinners)) & pieces();

		if (b.equal_to_two())
			result |= b & pieces(us);
	}

	//knight
	pinners = pieces(us, KNIGHT) & KnightAttackMask[ksq];
	while (pinners) {
		//pinner马脚与ksq马眼的交集正好是蹩腿的子
		b = (KnightLeg[pop_lsb(&pinners)] & KnightEye[ksq])& pieces();
		result |= b & pieces(us);
	}

	return result;
}
Bitboard Position::discovered_cannon_check_candidates() const
{
	//该函数主要计算我方炮架位置

	Bitboard b;
	Color us = side_to_move();
	Color them = ~us;
	Square ksq = king_square(them);


	//用rook的方法计算这种情况
	//如果存在我方cannon与对方的king face to face 的情况
	Bitboard cannons = attacks_from<ROOK>(ksq) & pieces(us, CANNON);
	while (cannons) {
		b |= between_bb(ksq, pop_lsb(&cannons));
	}

	return b;
}

Bitboard Position::discovered_cannon_face_king() const
{
   //them cannon and us king face to face
   //us move to fibiden area

	Bitboard b;
	Color us = side_to_move();
	Color them = ~us;
	Square ksq = king_square(us);

	//我方的cannon与敌方的king face to face的情况
	Bitboard cannons = attacks_from<ROOK>(ksq) & pieces(them, CANNON);
	while (cannons) {
		b |= between_bb(ksq, pop_lsb(&cannons));
	}

	return b;
}

bool Position::legal(Move m, const Bitboard& pinned) {

	assert(is_ok(m));
	assert(pinned == pinned_pieces(sideToMove));

	Color us = sideToMove;
	Square from = from_sq(m);

	assert(piece_on(king_square(us)) == make_piece(us, KING));

	ExtMove move;
	move.move = m;	

	// If the moving piece is a king, check whether the destination
	// square is attacked by the opponent.
	if (type_of(piece_on(from)) == KING)
	{
		//return !(attackers_to(to_sq(m)) & pieces(~us));
		return move_is_legal((*this), move);
	}
		

	// A non-king move is legal if and only if it is not pinned or it
	// is moving along the ray towards or away from the king.
	if (!pinned || !(pinned & from))	return true; 

	return move_is_legal((*this), move);

}
Bitboard Position::attackers_to(Square s) const {

	return (pawn_attacks_from(BLACK, s) & pieces(BLACK, PAWN))
		| (pawn_attacks_from(WHITE, s) & pieces(WHITE, PAWN))
		| (cannon_control_bb(s, occupied) & (pieces(WHITE, CANNON) | pieces(BLACK, CANNON)))
		| (rook_attacks_bb(s, occupied) & (pieces(WHITE, ROOK) | pieces(BLACK, ROOK)))
		| (knight_attacks_from(s, occupied) & (pieces(WHITE, KNIGHT) | pieces(BLACK, KNIGHT)))
		| (attacks_from<BISHOP>(s) &  (pieces(WHITE, BISHOP) | pieces(BLACK, BISHOP)))
		| (attacks_from<ADVISOR>(s) &  (pieces(WHITE, ADVISOR) | pieces(BLACK, ADVISOR)))
		| (attacks_from<KING>(s) &  (pieces(WHITE, KING) | pieces(BLACK, KING)));

}
Bitboard Position::checkers(Color checker, Square ksq) const {
	return pawn_attacks_from(checker, ksq) & pieces(checker, PAWN)
		| cannon_control_bb(ksq, occupied) & pieces(checker, CANNON)
		| rook_attacks_bb(ksq, occupied) &  pieces(checker, ROOK)
		| knight_attacks_from(ksq, occupied) & pieces(checker, KNIGHT);		 
}
bool Position::gives_check(Move m, const CheckInfo& ci) const {

	assert(is_ok(m));
	assert(ci.dcCandidates == discovered_check_candidates());
	//assert(color_of(moved_piece(m)) == sideToMove);

	Square from = from_sq(m);
	Square to = to_sq(m);
	PieceType pt = type_of(piece_on(from));

	//cannon非常特殊，如果准确判断，可以针对cannon特殊处理


	// Is there a direct check?
	if (ci.checkSq[pt] & to)
		return true; //也不是非常确定，也有例外,对cannon翻打没有

	// Is there a discovered check?
	//if (ci.dcCandidates
	//	&& (ci.dcCandidates & from)
	//	&& !aligned(from, to, ci.ksq))
	//	return true;
	 //这里准确检查是否将军开销很大，
	//在ci中增加更多信息或许可行
	if (ci.dcCandidates && (ci.dcCandidates & from))
	{
		//也不是非常确定，from沿着 candidate与king在同一线上移动
		return true;
	}
	else if (ci.dcCannonCandidates && (ci.dcCannonCandidates & to))
	{
		//也不是非常确定 ，from沿着 candidate与king在同一线上移动
		return true;
	}
	else if (pt == CANNON && /*RookAttackMask[from] & */RookAttackMask[to] & ci.ksq) {
		//情况复杂，如果这里认真判断，不如在do_move的之后直接计算checkerBB
		//如果search中需要该函数准确判断，可写另外一个函数
		//cannon与king在同一线，翻打会出现将军
		return true;
	}

	return false;
}
void Position::do_move(Move m, StateInfo& newSt, const CheckInfo& ci, bool moveIsCheck) {

	assert(is_ok(m));
	assert(&newSt != st);

	// Copy some fields of the old state to our new StateInfo object except the
	// ones which are going to be recalculated from scratch anyway and then switch
	// our state pointer to point to the new (ready to be updated) state.
	std::memcpy(&newSt, st, sizeof(StateInfo));

	newSt.previous = st;
	st = &newSt;


	// Increment ply counters. In particular, rule50 will be reset to zero later on
	// in case of a capture or a pawn move.
	++gamePly;

	Color us = sideToMove;
	Color them = ~us;
	Square from = from_sq(m);
	Square to = to_sq(m);
	Piece pc = piece_on(from);
	PieceType pt = type_of(pc);
	PieceType captured = type_of(piece_on(to));

	assert(color_of(pc) == us);
	assert(piece_on(to) == NO_PIECE || color_of(piece_on(to)) == them);
	assert(captured != KING);

	if (captured)
	{
		remove_piece(to, them, captured);
	}

	move_piece(from, to, us, pt);

	// Set capture piece
	st->capturedType = captured;

	// Update checkers bitboard: piece must be already moved due to attacks_from()
	st->checkersBB = Bitboard();

	if (moveIsCheck)
	{
		st->checkersBB = checkers(us, king_square(them));;// attackers_to(king_square(them)) & pieces(us);

	}
	//for debug
	//else {
	//	if (checkers(us, king_square(them))) {
	//		printf("%s\n", pretty().c_str());
	//		printf("%s ", move_to_chinese(*this, m).c_str());
	//		
	//		ci.checkSq[pt].print(stdout);
	//		ci.dcCannonCandidates.print(stdout);
	//		ci.dcCandidates.print(stdout);
	//		
	//		getchar();

	//		gives_check(m, ci);
	//	}
	//}

	sideToMove = ~sideToMove;

	assert(pos_is_ok());
}
void Position::undo_move(Move m) {

	assert(is_ok(m));

	sideToMove = ~sideToMove;

	Color us = sideToMove;
	Square from = from_sq(m);
	Square to = to_sq(m);
	PieceType pt = type_of(piece_on(to));

	assert(st->capturedType != KING);

	move_piece(to, from, us, pt); // Put the piece back at the source square

	if (st->capturedType)
	{
		Square capsq = to;

		put_piece(capsq, ~us, st->capturedType); // Restore the captured piece
	}


	// Finally point our state pointer back to the previous state
	st = st->previous;
	--gamePly;

	assert(pos_is_ok());
}
