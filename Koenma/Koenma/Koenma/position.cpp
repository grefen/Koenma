/*
Koenma, a UCI chess playing engine derived from Stockfish 7
Copyright (C) 2016-2017 grefen

Koenma is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Koenma is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>
#include <cassert>
#include <cstring>   // For std::memset, std::memcmp
#include <iomanip>
#include <sstream>
#include <iostream>
#include <sstream>
#include <string>

#include "bitcount.h"
#include "misc.h"
#include "movegen.h"
#include "position.h"
#include "thread.h"
#include "tt.h"
//#include "uci.h"

using std::string;

Value PieceValue[PHASE_NB][PIECE_NB] = {
{ VALUE_ZERO, PawnValueMg, AdvisorValueMg, BishopValueMg, KnightValueMg, CannonValueMg, RookValueMg},
{ VALUE_ZERO, PawnValueEg, AdvisorValueEg, BishopValueEg, KnightValueEg, CannonValueEg, RookValueEg} };


namespace Zobrist {

	Key psq[COLOR_NB][PIECE_TYPE_NB][SQUARE_NB];

	Key side;
	Key exclusion;
}

Key Position::exclusion_key() const { return st->key ^ Zobrist::exclusion; }

namespace {

	static const string PieceToChar(" PABNCRK pabncrk");

	// min_attacker() is a helper function used by see() to locate the least
	// valuable attacker for the side to move, remove the attacker we just found
	// from the bitboards and scan for new X-ray attacks behind it.
	//bb:byTypeBB
	//to: attack target
	//stmAttackers: from stmAttackers find a attacker that with min value
	//occupied: current pos occ, where the from piece removed 
	//attackers: all piece which attact to to include while and black piece
	template<int Pt>
	PieceType min_attacker(const Bitboard* bb, Square to, Bitboard stmAttackers,
		Bitboard& occupied, Bitboard& attackers) {

		Bitboard b = stmAttackers & bb[Pt];
		if (!b)//  Pt may not attack to, find next pt
			return min_attacker<Pt + 1>(bb, to, stmAttackers, occupied, attackers);

		//occupied ^= b & ~(b - 1);
		occupied ^= lsb(b);

		//find attacker that attack to 'to'
		//由于pawn advisor，king不涉及到蹩腿，并且不是长距离攻击的子，所以在调用这个
		//函数以前attackers中已经包含了，所以该函数不需要计算
		//该函数主要计算蹩腿和长距离攻击的子
		//马如果在调用这个函数之前不是attacker,那么就不可能是attacker
		//因为其它attacker的移动不会对马产生影响
		//bishop道理是一样的
		//所以主要计算 1 pawn移动后，后面是否有rook和cannon
		//2 cannon移动后，是否还有cannon
		//3 rook移动后，后面是否有cannon和rook
		//该函数没有考虑pinned的情况和king face to face
		if (Pt == PAWN)
		{
			attackers |= (rook_attacks_bb(to, occupied) & bb[ROOK]);
			attackers |= (cannon_attacks_bb(to, occupied) & bb[CANNON]);
		}
		else if (Pt == CANNON)
		{
			attackers |= (cannon_attacks_bb(to, occupied) & bb[CANNON]);
			//如果是cannon attack,就不可能是rook,中间有炮架
		}
		else if (Pt == ROOK)
		{
			attackers |= (rook_attacks_bb(to, occupied) & bb[ROOK]);
			attackers |= (cannon_attacks_bb(to, occupied) & bb[CANNON]);
		}

		attackers &= occupied; // After X-ray that may add already processed pieces
		return (PieceType)Pt;
	}

	template<>
	PieceType min_attacker<KING>(const Bitboard*, Square, Bitboard, Bitboard&, Bitboard&) {
		return KING; // No need to update bitboards: it is the last cycle
	}

} // namespace


/// CheckInfo constructor

CheckInfo::CheckInfo(const Position& pos) {

	Color them = ~pos.side_to_move();
	ksq = pos.square<KING>(them);

	pinned = pos.pinned_pieces(pos.side_to_move());	 //我方被牵制的子
	dcCandidates = pos.discovered_check_candidates();  //我方移动后将军的子，一般是牵制和马腿，炮架
	dcCannonCandidates = pos.discovered_cannon_check_candidates(); //炮和对方king之间的位置

	//下面是直接将军的位置，cannon不能找到所有情况
	checkSquares[PAWN] = PawnAttackTo[~them][ksq];
	checkSquares[KNIGHT] = knight_eye_attacks_bb(ksq, pos.pieces());
	checkSquares[ROOK] = pos.attacks_from<ROOK>(ksq);
	//cannon不能够把所有情况找到,
	//有炮架，通过 dcCannonCandidates
	//cannon 与 king在同一线上，cannon翻打会出现将军的情况(最为复杂)
	checkSquares[CANNON] = pos.attacks_from<CANNON>(ksq);

	checkSquares[BISHOP] = Bitboard();
	checkSquares[ADVISOR] = Bitboard();
	checkSquares[KING] = Bitboard();
}


/// operator<<(Position) returns an ASCII representation of the position

std::ostream& operator<<(std::ostream& os, const Position& pos) {

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

	for (Bitboard b = pos.pieces(); b; )
	{
		Square s = pop_lsb(&b);

		int i = 646 - 34 * (rank_of(s) * 2) - 34 + 4 * file_of(s);
		int c = PieceToChar[pos.piece_on(s)];

		brd[i] = c;
	}

	os << brd << "\n";

	os << "\nFen: " << pos.fen() << "\nKey: " << std::hex << std::uppercase
		<< std::setfill('0') << std::setw(16) << pos.key() << std::dec << "\nCheckers: ";

	//for (Bitboard b = pos.checkers(); b; )
	//	os << UCI::square(pop_lsb(&b)) << " ";

	return os;
}


/// Position::init() initializes at startup the various arrays used to compute
/// hash keys.

void Position::init() {

	PRNG rng(1070372);

	for (Color c = WHITE; c <= BLACK; ++c)
		for (PieceType pt = PAWN; pt <= KING; ++pt)
			for (Square s = SQ_A0; s <= SQ_I9; ++s)
				Zobrist::psq[c][pt][s] = rng.rand<Key>();


	Zobrist::side = rng.rand<Key>();
	Zobrist::exclusion = rng.rand<Key>();
}


/// Position::operator=() creates a copy of 'pos' but detaching the state pointer
/// from the source to be self-consistent and not depending on any external data.

Position& Position::operator=(const Position& pos) {

	std::memcpy(this, &pos, sizeof(Position));
	std::memcpy(&startState, st, sizeof(StateInfo));
	st = &startState;
	nodes = 0;

	assert(pos_is_ok());

	return *this;
}


/// Position::clear() erases the position object to a pristine state, with an
/// empty board, white to move.

void Position::clear() {

	std::memset(this, 0, sizeof(Position));

	st = &startState;

	for (int i = 0; i < PIECE_TYPE_NB; ++i)
		for (int j = 0; j < 16; ++j)
			pieceList[WHITE][i][j] = pieceList[BLACK][i][j] = SQ_NONE;
}


/// Position::set() initializes the position object with the given FEN string.
/// This function is not very robust - make sure that input FENs are correct,
/// this is assumed to be the responsibility of the GUI.

void Position::set(const string& fenStr, bool isChess960, Thread* th) {
	/*
	   A FEN string defines a particular position using only the ASCII character set.

	   A FEN string contains six fields separated by a space. The fields are:

	   1) Piece placement (from white's perspective). Each rank is described, starting
		  with rank 8 and ending with rank 1. Within each rank, the contents of each
		  square are described from file A through file H. Following the Standard
		  Algebraic Notation (SAN), each piece is identified by a single letter taken
		  from the standard English names. White pieces are designated using upper-case
		  letters ("PNBRQK") whilst Black uses lowercase ("pnbrqk"). Blank squares are
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
			put_piece(color_of(Piece(p)), type_of(Piece(p)), sq);
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
	ss >> std::skipws >> st->rule50 >> gamePly;

	// Convert from fullmove starting from 1 to ply starting from 0,
	// handle also common incorrect FEN with fullmove = 0.
	gamePly = std::max(2 * (gamePly - 1), 0) + (sideToMove == BLACK);


	thisThread = th;
	set_state(st);

	assert(pos_is_ok());
}

/// Position::set_state() computes the hash keys of the position, and other
/// data that once computed is updated incrementally as moves are made.
/// The function is only used when a new position is set up, and to verify
/// the correctness of the StateInfo data when running in debug mode.

void Position::set_state(StateInfo* si) const {

	si->key = si->pawnKey = si->materialKey = 0;
	si->nonPawnMaterial[WHITE] = si->nonPawnMaterial[BLACK] = VALUE_ZERO;
	si->psq = SCORE_ZERO;

	si->checkersBB = attackers_to(square<KING>(sideToMove)) & pieces(~sideToMove);

	for (Bitboard b = pieces(); b; )
	{
		Square s = pop_lsb(&b);
		Piece pc = piece_on(s);
		si->key ^= Zobrist::psq[color_of(pc)][type_of(pc)][s];
		si->psq += PSQT::psq[color_of(pc)][type_of(pc)][s];
	}


	if (sideToMove == BLACK)
		si->key ^= Zobrist::side;



	for (Bitboard b = pieces(PAWN); b; )
	{
		Square s = pop_lsb(&b);
		si->pawnKey ^= Zobrist::psq[color_of(piece_on(s))][PAWN][s];
	}

	for (Color c = WHITE; c <= BLACK; ++c)
		for (PieceType pt = PAWN; pt <= KING; ++pt)
			for (int cnt = 0; cnt < pieceCount[c][pt]; ++cnt)
				si->materialKey ^= Zobrist::psq[c][pt][cnt];

	for (Color c = WHITE; c <= BLACK; ++c)
		for (PieceType pt = ADVISOR; pt <= ROOK; ++pt)
			si->nonPawnMaterial[c] += pieceCount[c][pt] * PieceValue[MG][pt];
}


/// Position::fen() returns a FEN representation of the position. In case of
/// Chess960 the Shredder-FEN notation is used. This is mainly a debugging function.

const string Position::fen() const {

	std::ostringstream ss;

	for (Rank rank = RANK_9; rank >= RANK_0; --rank)
	{
		for (File file = FILE_A; file <= FILE_I; ++file)
		{
			Square sq = make_square(file, rank);

			if (empty(sq))
			{
				int emptyCnt = 1;

				for (; file < FILE_I && empty(++sq); ++file)
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

	ss << "-";
	ss << " ";
	ss << "-";
	ss << " ";
	ss << st->rule50 << " " << 1 + (gamePly - int(sideToMove == BLACK)) / 2;

	return ss.str();
}


/// Position::game_phase() calculates the game phase interpolating total non-pawn
/// material between endgame and midgame limits.

Phase Position::game_phase() const {

	Value npm = st->nonPawnMaterial[WHITE] + st->nonPawnMaterial[BLACK];

	npm = std::max(EndgameLimit, std::min(npm, MidgameLimit));

	return Phase(((npm - EndgameLimit) * PHASE_MIDGAME) / (MidgameLimit - EndgameLimit));
}


/// Position::check_blockers() returns a bitboard of all the pieces with color
/// 'c' that are blocking check on the king with color 'kingColor'. A piece
/// blocks a check if removing that piece from the board would result in a
/// position where the king is in check. A check blocking piece can be a
/// pinned piece.
///only pinned piece,not be candidate check, because king face to king case  
Bitboard Position::check_blockers(Color c, Color kingColor) const {

	Bitboard b, pinners, result;
	Square ksq = square<KING>(kingColor);

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

//me check them king
Bitboard Position::discovered_check_candidates() const
{
	Bitboard b, pinners, result;
	Color us = side_to_move();
	Color them = ~us;
	Square ksq = square<KING>(them);


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
	Square ksq = square<KING>(them);


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
	Square ksq = square<KING>(us);

	//我方的cannon与敌方的king face to face的情况
	Bitboard cannons = attacks_from<ROOK>(ksq) & pieces(them, CANNON);
	while (cannons) {
		b |= between_bb(ksq, pop_lsb(&cannons));
	}

	return b;
}
/// Position::attackers_to() computes a bitboard of all pieces which attack a
/// given square. Slider attacks use the occupied bitboard to indicate occupancy.

Bitboard Position::attackers_to(Square s, Bitboard occupied) const {

	return (PawnAttackTo[BLACK][s] & pieces(BLACK, PAWN))
		| (PawnAttackTo[WHITE][s] & pieces(WHITE, PAWN))
		| (rook_attacks_bb(s, occupied) & pieces(ROOK))
		| (cannon_attacks_bb(s, occupied) & pieces(CANNON))
		| (knight_eye_attacks_bb(s, occupied) & pieces(KNIGHT))
		| (bishop_attacks_bb(s, occupied) & pieces(BISHOP))
		| (AdvisorAttack[s] & pieces(ADVISOR))
		| (KingAttack[s] & pieces(KING));
}


/// Position::legal() tests whether a pseudo-legal move is legal

bool Position::legal(Move m, Bitboard pinned) const {

	assert(is_ok(m));
	assert(pinned == pinned_pieces(sideToMove));
	//in search will use this function, must be sure is ok 
	//return move_is_legal((*this), m);	// ok, but a bit sollow
#if 1
	Color us = sideToMove;
	Square from = from_sq(m);
	Square to = to_sq(m);

	assert(color_of(moved_piece(m)) == us);
	assert(piece_on(square<KING>(us)) == make_piece(us, KING));

	// If the moving piece is a king, check whether the destination
	// square is attacked by the opponent. Castling moves are checked
	// for legality during move generation.
	if (type_of(piece_on(from)) == KING)
	{
		//return !(attackers_to(to_sq(m)) & pieces(~us));
		return move_is_legal((*this), m);
	}

	if (discovered_cannon_face_king() & to)
	{
		return move_is_legal((*this), m);
	}

	// A non-king move is legal if and only if it is not pinned or it
	// is moving along the ray towards or away from the king.

	if (!pinned || !(pinned & from))	return true;

	return move_is_legal((*this), m);
#endif
}


/// Position::pseudo_legal() takes a random move and tests whether the move is
/// pseudo legal. It is used to validate moves from TT that can be corrupted
/// due to SMP concurrent access or hash position key aliasing.

bool Position::pseudo_legal(const Move m) const {

	Color us = sideToMove;
	Square from = from_sq(m);
	Square to = to_sq(m);
	Piece pc = moved_piece(m);
	Piece topc = piece_on(to);

	// If the 'from' square is not occupied by a piece belonging to the side to
	// move, the move is obviously not legal.
	if (pc == NO_PIECE || color_of(pc) != us)
		return false;

	// The destination square cannot be occupied by a friendly piece
	if (pieces(us) & to)
		return false;

	//虽然move合理，可能出现主动被将军
	//走到对方炮架
	//pinned的子移开
	//king移到对方攻击点

	// 1 招法合理 2 不被将军

	// Handle the special case of a pawn move
	if (type_of(pc) == PAWN)
	{
		if (!(attacks_from<PAWN>(from, us)&to))
			return false;
	}
	else if (type_of(pc) == CANNON)
	{
		Bitboard att = attacks_from<CANNON>(from) & pieces(~us);
		if ((piece_on(to) != NO_PIECE) && !(att & to))
			return false;

		Bitboard natt = attacks_from<ROOK>(from)& (~pieces());
		if((piece_on(to) == NO_PIECE) && !(natt & to))
			return false;

		if (!((att | natt) & to)) return false;
		if ( (att&to) && (piece_on(to) == NO_PIECE))	return false;
		if ( (natt&to) && (piece_on(to) != NO_PIECE)) return false;
	}
	else if (!(attacks_from(pc, from) & to))
	{
		return false;
	}

	return move_is_legal((*this), m);
}


/// Position::gives_check() tests whether a pseudo-legal move gives a check

bool Position::gives_check(Move m, const CheckInfo& ci) const {

	assert(is_ok(m));
	assert(ci.dcCandidates == discovered_check_candidates());
	assert(color_of(moved_piece(m)) == sideToMove);

	Square from = from_sq(m);
	Square to = to_sq(m);
	PieceType pt = type_of(piece_on(from));

	// Is there a direct check?
	if (ci.checkSquares[pt] & to)
		return true;

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
	//该函数只是判断可能出现check，不是很准确，如果很准确，开销会很大
	//该函数主要用于do_move中计算checkerBB

	return false;
}


/// Position::do_move() makes a move, and saves all information necessary
/// to a StateInfo object. The move is assumed to be legal. Pseudo-legal
/// moves should be filtered out before this function is called.

void Position::do_move(Move m, StateInfo& newSt, bool givesCheck) {

	assert(is_ok(m));
	assert(&newSt != st);

	++nodes;
	Key k = st->key ^ Zobrist::side;

	// Copy some fields of the old state to our new StateInfo object except the
	// ones which are going to be recalculated from scratch anyway and then switch
	// our state pointer to point to the new (ready to be updated) state.
	std::memcpy(&newSt, st, offsetof(StateInfo, key));
	newSt.previous = st;
	st = &newSt;

	// Increment ply counters. In particular, rule50 will be reset to zero later on
	// in case of a capture or a pawn move.
	++gamePly;
	++st->rule50;
	++st->pliesFromNull;

	Color us = sideToMove;
	Color them = ~us;
	Square from = from_sq(m);
	Square to = to_sq(m);
	PieceType pt = type_of(piece_on(from));
	PieceType captured = type_of(piece_on(to));

	assert(color_of(piece_on(from)) == us);
	assert(piece_on(to) == NO_PIECE || color_of(piece_on(to)) == them);
	assert(captured != KING);

	if (captured)
	{
		Square capsq = to;

		// If the captured piece is a pawn, update pawn hash key, otherwise
		// update non-pawn material.
		if (captured == PAWN)
		{
			st->pawnKey ^= Zobrist::psq[them][PAWN][capsq];
		}
		else
			st->nonPawnMaterial[them] -= PieceValue[MG][captured];

		// Update board and piece lists
		remove_piece(them, captured, capsq);

		// Update material hash key and prefetch access to materialTable
		k ^= Zobrist::psq[them][captured][capsq];
		st->materialKey ^= Zobrist::psq[them][captured][pieceCount[them][captured]];
		//prefetch(thisThread->materialTable[st->materialKey]);	

		// Update incremental scores
		st->psq -= PSQT::psq[them][captured][capsq];

		// Reset rule 50 counter
		st->rule50 = 0;
	}

	// Update hash key
	k ^= Zobrist::psq[us][pt][from] ^ Zobrist::psq[us][pt][to];

	// Move the piece.	
	move_piece(us, pt, from, to);

	// If the moving piece is a pawn do some special extra work
	if (pt == PAWN)
	{
		// Update pawn hash key and prefetch access to pawnsTable
		st->pawnKey ^= Zobrist::psq[us][PAWN][from] ^ Zobrist::psq[us][PAWN][to];
		//prefetch(thisThread->pawnsTable[st->pawnKey]);

		// Reset rule 50 draw counter
		//st->rule50 = 0;
	}

	// Update incremental scores
	st->psq += PSQT::psq[us][pt][to] - PSQT::psq[us][pt][from];

	// Set capture piece
	st->capturedType = captured;

	// Update the key with the final value
	st->key = k;

	// Calculate checkers bitboard (if move gives check)
	st->checkersBB = get_checkers(us, square<KING>(them));//givesCheck ? get_checkers(us, square<KING>(them)) : Bitboard(); // 
	
	sideToMove = ~sideToMove;

	assert(!!st->checkersBB == in_check(sideToMove));
	
	assert(pos_is_ok());
}
//将军的bitboard
Bitboard Position::get_checkers(Color checker, Square ksq) const {

	return	(PawnAttackTo[checker][ksq] & pieces(checker, PAWN))
		| (rook_attacks_bb(ksq, byTypeBB[ALL_PIECES]) & pieces(checker, ROOK))
		| (cannon_attacks_bb(ksq, byTypeBB[ALL_PIECES]) & pieces(checker, CANNON))
		| (knight_eye_attacks_bb(ksq, byTypeBB[ALL_PIECES]) & pieces(checker, KNIGHT));

}

bool Position::in_check(Color c)
{
	Square king = square<KING>(c);
	Color  opp = ~c;

	Bitboard rooks = pieces(opp, ROOK);
	Bitboard knights = pieces(opp, KNIGHT);
	Bitboard cannons = pieces(opp, CANNON);
	Bitboard pawns = pieces(opp, PAWN);

	if ((rook_attacks_bb(king, pieces()) & rooks)) return true;
	if ((knight_eye_attacks_bb(king, pieces()) & knights)) return true;
	if ((cannon_attacks_bb(king, pieces()) & cannons)) return true;
	if ((PawnAttackTo[opp][king] & pawns)) return true;
	if ((rook_attacks_bb(king, pieces()) & square<KING>(opp))) return true;

	return false;
}
/// Position::undo_move() unmakes a move. When it returns, the position should
/// be restored to exactly the same state as before the move was made.

void Position::undo_move(Move m) {

	assert(is_ok(m));

	sideToMove = ~sideToMove;

	Color us = sideToMove;
	Square from = from_sq(m);
	Square to = to_sq(m);
	PieceType pt = type_of(piece_on(to));

	assert(empty(from));
	assert(st->capturedType != KING);


	move_piece(us, pt, to, from); // Put the piece back at the source square

	if (st->capturedType)
	{
		Square capsq = to;

		put_piece(~us, st->capturedType, capsq); // Restore the captured piece
	}


	// Finally point our state pointer back to the previous state
	st = st->previous;
	--gamePly;

	assert(pos_is_ok());
}

/// Position::do(undo)_null_move() is used to do(undo) a "null move": It flips
/// the side to move without executing any move on the board.

void Position::do_null_move(StateInfo& newSt) {

	assert(!checkers());
	assert(!in_check(~side_to_move()));
	assert(&newSt != st);

	std::memcpy(&newSt, st, sizeof(StateInfo));
	newSt.previous = st;
	st = &newSt;

	st->key ^= Zobrist::side;
	prefetch(TT.first_entry(st->key)); 

	++st->rule50;
	st->pliesFromNull = 0;

	sideToMove = ~sideToMove;

	assert(pos_is_ok());
}

void Position::undo_null_move() {

	assert(!checkers());

	st = st->previous;
	sideToMove = ~sideToMove;
}


/// Position::key_after() computes the new hash key after the given move. Needed
/// for speculative prefetch. It doesn't recognize special moves like castling,
/// en-passant and promotions.

Key Position::key_after(Move m) const {

	Color us = sideToMove;
	Square from = from_sq(m);
	Square to = to_sq(m);
	PieceType pt = type_of(piece_on(from));
	PieceType captured = type_of(piece_on(to));
	Key k = st->key ^ Zobrist::side;

	if (captured)
		k ^= Zobrist::psq[~us][captured][to];

	return k ^ Zobrist::psq[us][pt][to] ^ Zobrist::psq[us][pt][from];
}


/// Position::see() is a static exchange evaluator: It tries to estimate the
/// material gain or loss resulting from a move.

Value Position::see_sign(Move m) const {

	assert(is_ok(m));

	// Early return if SEE cannot be negative because captured piece value
	// is not less then capturing one. Note that king moves always return
	// here because king midgame value is set to 0.
	if (PieceValue[MG][moved_piece(m)] <= PieceValue[MG][piece_on(to_sq(m))])
		return VALUE_KNOWN_WIN;

	return see(m);
}

Value Position::see(Move m) const {

	Square from, to;
	Bitboard occupied, attackers, stmAttackers;
	Value swapList[32];
	int slIndex = 1;
	PieceType captured;
	Color stm;

	assert(is_ok(m));

	from = from_sq(m);
	to = to_sq(m);
	swapList[0] = PieceValue[MG][piece_on(to)];
	stm = color_of(piece_on(from));
	occupied = pieces() ^ from;

	// Find all attackers to the destination square, with the moving piece
	// removed, but possibly an X-ray attacker added behind it.
	attackers = attackers_to(to, occupied) & occupied;

	// If the opponent has no attackers we are finished
	stm = ~stm;
	stmAttackers = attackers & pieces(stm);
	if (!stmAttackers)
		return swapList[0];

	// The destination square is defended, which makes things rather more
	// difficult to compute. We proceed by building up a "swap list" containing
	// the material gain or loss at each stop in a sequence of captures to the
	// destination square, where the sides alternately capture, and always
	// capture with the least valuable piece. After each capture, we look for
	// new X-ray attacks from behind the capturing piece.
	captured = type_of(piece_on(from));

	do {
		assert(slIndex < 32);

		// Add the new entry to the swap list
		swapList[slIndex] = -swapList[slIndex - 1] + PieceValue[MG][captured];

		// Locate and remove the next least valuable attacker
		captured = min_attacker<PAWN>(byTypeBB, to, stmAttackers, occupied, attackers);
		stm = ~stm;
		stmAttackers = attackers & pieces(stm);
		++slIndex;

	} while (stmAttackers && (captured != KING || (--slIndex, false))); // Stop before a king capture

	// Having built the swap list, we negamax through it to find the best
	// achievable score from the point of view of the side to move.
	while (--slIndex)
		swapList[slIndex - 1] = std::min(-swapList[slIndex], swapList[slIndex - 1]);

	return swapList[0];
}


/// Position::is_draw() tests whether the position is drawn by 50-move rule
/// or by repetition. It does not detect stalemates.

bool Position::is_draw() const {

	if (st->rule50 > 99 && (!checkers() || MoveList<LEGAL>(*this).size()))
		return true;

	StateInfo* stp = st;
	for (int i = 2, e = std::min(st->rule50, st->pliesFromNull); i <= e; i += 2)
	{
		stp = stp->previous->previous;

		if (stp->key == st->key)
			return true; // Draw at first repetition
	}

	return false;
}

int  Position::is_repeat()const
{	
	bool oppcheck = true;
	bool mecheck = true;
	int  checktime = 0;

	StateInfo* stp = st;
	for (int i = 2, e = std::min(st->rule50, st->pliesFromNull); i <= e; i += 2)
	{
		StateInfo* stopp = stp->previous;
		StateInfo* stme = stp;
		
		stp = stp->previous->previous;

		if (oppcheck && !stme->checkersBB) oppcheck = false;
		if (mecheck && !stopp->checkersBB)  mecheck = false;

		if (stp->key == st->key)  //repetition
		{
			checktime++;

			if (checktime >= 1)
			{
				return REPEATE_TRUE + (mecheck ? REPEATE_ME_CHECK : REPEATE_NONE) + (oppcheck ? REPEATE_OPP_CHECK : REPEATE_NONE);
			}
		}
	}

	return REPEATE_NONE;
}
/// Position::flip() flips position with the white and black sides reversed. This
/// is only useful for debugging e.g. for finding evaluation symmetry bugs.

void Position::flip() {

	string f, token;
	std::stringstream ss(fen());

	for (Rank r = RANK_9; r >= RANK_0; --r) // Piece placement
	{
		std::getline(ss, token, r > RANK_0 ? '/' : ' ');
		f.insert(0, token + (f.empty() ? " " : "/"));
	}

	ss >> token; // Active color
	f += (token == "w" ? "B " : "W "); // Will be lowercased later

	ss >> token; // Castling availability
	f += token + " ";

	std::transform(f.begin(), f.end(), f.begin(),
		[](char c) { return char(islower(c) ? toupper(c) : tolower(c)); });

	ss >> token; // En passant square
	f += (token == "-" ? token : token.replace(1, 1, token[1] == '3' ? "6" : "3"));

	std::getline(ss, token); // Half and full moves
	f += token;

	set(f, false, this_thread());

	assert(pos_is_ok());
}


/// Position::pos_is_ok() performs some consistency checks for the position object.
/// This is meant to be helpful when debugging.

bool Position::pos_is_ok(int* failedStep) const {

	const bool Fast = false; // Quick (default) or full check?

	enum { Default, King, Bitboards, State, Lists };

	for (int step = Default; step <= (Fast ? Default : Lists); step++)
	{
		if (failedStep)
			*failedStep = step;

		if (step == Default)
			if ((sideToMove != WHITE && sideToMove != BLACK)
				|| piece_on(square<KING>(WHITE)) != W_KING
				|| piece_on(square<KING>(BLACK)) != B_KING
				)
				return false;

		if (step == King)
		{
			if (std::count(board, board + SQUARE_NB, W_KING) != 1
				|| std::count(board, board + SQUARE_NB, B_KING) != 1
				|| attackers_to(square<KING>(~sideToMove)) & pieces(sideToMove))
				return false;

			if (rook_attacks_bb(square<KING>(~sideToMove), pieces()) & square<KING>(sideToMove))
				return false;
		}

		if (step == Bitboards)
		{
			if ((pieces(WHITE) & pieces(BLACK))
				|| (pieces(WHITE) | pieces(BLACK)) != pieces())
				return false;

			for (PieceType p1 = PAWN; p1 <= KING; ++p1)
				for (PieceType p2 = PAWN; p2 <= KING; ++p2)
					if (p1 != p2 && (pieces(p1) & pieces(p2)))
						return false;
		}

		if (step == State)
		{
			StateInfo si = *st;
			set_state(&si);
			if (std::memcmp(&si, st, sizeof(StateInfo)))
				return false;
		}

		if (step == Lists)
			for (Color c = WHITE; c <= BLACK; ++c)
				for (PieceType pt = PAWN; pt <= KING; ++pt)
				{
					if (pieceCount[c][pt] != popcount(pieces(c, pt)))
						return false;

					for (int i = 0; i < pieceCount[c][pt]; ++i)
						if (board[pieceList[c][pt][i]] != make_piece(c, pt)
							|| index[pieceList[c][pt][i]] != i)
							return false;
				}
	}

	return true;
}
