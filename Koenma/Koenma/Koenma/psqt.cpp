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

#include "types.h"

namespace PSQT {

#define S(mg, eg) make_score(mg, eg)

// Bonus[PieceType][Square / 2] contains Piece-Square scores. For each piece
// type on a given square a (middlegame, endgame) score pair is assigned. Table
// is defined for files A..D and white side: it is symmetric for black side and
// second half of the files.
const Score Bonus[][RANK_NB][int(FILE_NB+1) / 2] = {
  { },
  { // Pawn
   { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
   { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
   { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
   { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
   { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	//---------------------------------
   { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
   { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
   { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
   { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
   { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) }
  },
  { // Advisor
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	//---------------------------------
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) }
  },
  { // Bishop
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	//---------------------------------
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) }
  },
  { // Knight
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	//---------------------------------
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) }
  },
  { // Cannon
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	//---------------------------------
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) }
  },
  { // Rook
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	//---------------------------------
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) }
  },
  { // King
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	//---------------------------------
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) },
	  { S(0,0), S(0,0), S(0,0), S(0,0), S(0,0) }
  }
};

#undef S

Score psq[COLOR_NB][PIECE_TYPE_NB][SQUARE_NB];

// init() initializes piece square tables: the white halves of the tables are
// copied from Bonus[] adding the piece value, then the black halves of the
// tables are initialized by flipping and changing the sign of the white scores.
void init() {

  for (PieceType pt = PAWN; pt <= KING; ++pt)
  {
      PieceValue[MG][make_piece(BLACK, pt)] = PieceValue[MG][pt];
      PieceValue[EG][make_piece(BLACK, pt)] = PieceValue[EG][pt];

      Score v = make_score(PieceValue[MG][pt], PieceValue[EG][pt]);

      for (Square s = SQ_A0; s <= SQ_I9; ++s)
      {
          int edgeDistance = file_of(s) <= FILE_E ? file_of(s) : FILE_I - file_of(s);
          psq[BLACK][pt][~s] = -(psq[WHITE][pt][s] = v + Bonus[pt][rank_of(s)][edgeDistance]);
      }
  }
}

} // namespace PSQT
