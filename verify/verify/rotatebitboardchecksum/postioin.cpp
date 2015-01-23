#include "position.h"
#include <cassert>
#include <cstddef>
#include <iostream>
#include <algorithm>
#include <sstream>


using std::string;
using std::cout;
using std::endl;

static const string PieceToChar(" PBANCRK pbancrk");


void Position::clear() {

	std::memset(this, 0, sizeof(Position));

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
	if (   ((ss >> col) && (col >= 'a' && col <= 'h'))
		&& ((ss >> row) && (row == '3' || row == '6')))
	{

	}

	// 5-6. Halfmove clock and fullmove number
	ss >> std::skipws >> rule50 >> gamePly;

	// Convert from fullmove starting from 1 to ply starting from 0,
	// handle also common incorrect FEN with fullmove = 0.
	gamePly = std::max(2 * (gamePly - 1), 0) + int(sideToMove == BLACK);


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

				for ( ; file < FILE_I && is_empty(++sq); ++file)
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

		int i = 646 - 34 * (rank_of(s)*2) - 34 + 4 * file_of(s);
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

void Position::do_move(Move m,undo_t* undo)
{
   	gamePly++;
	Color us = sideToMove;
	Color them = ~us;
	Square from = from_sq(m);
	Square to = to_sq(m);
	Piece pc = piece_on(from);
	PieceType pt = type_of(pc);
	PieceType capture = type_of(piece_on(to));

	if (capture)
	{
		remove_piece(to, them, capture);
	}

	move_piece(from, to, us, pt);

	undo->capturedType = capture;

	sideToMove = ~sideToMove;	

}
void Position::undo_move(Move m,undo_t* undo)
{
	assert(is_ok(m));

	sideToMove = ~sideToMove;

	Color us = sideToMove;
	Color them = ~us;
	Square from = from_sq(m);
	Square to = to_sq(m);
	PieceType pt = type_of(piece_on(to));
	PieceType capture = undo->capturedType;

	assert(capture != KING);

	move_piece(to, from, us, pt); // Put the piece back at the source square

	if (capture)
	{
		Square capsq = to;

		put_piece(capsq, them, capture); // Restore the captured piece
	}

	gamePly--;

	assert(pos_is_ok());
}
bool Position::in_check(Color c)
{    
	Square king = king_square(c);
	Color  opp = ~c;

	Bitboard rooks = pieces(opp, ROOK);
	Bitboard knights = pieces(opp, KNIGHT);
	Bitboard cannons = pieces(opp, CANNON);
	Bitboard pawns   = pieces(opp, PAWN);

	if((rook_attacks_bb(king,occupied,occupied_rl90) & rooks) ) return true;
	if((knight_attacks_from(king, occupied,occupied_rl90) & knights) ) return true;
	if((cannon_control_bb(king, occupied,occupied_rl90) & cannons)) return true;	
	if((pawn_attacks_from(opp, king) & pawns) ) return true;
    if((rook_attacks_bb(king,occupied,occupied_rl90) & king_square(opp)) ) return true;

	return false;
}
