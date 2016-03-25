#ifndef TYPES_H
#define TYPES_H
#include "platform.h"

#define CACHE_LINE_SIZE 128
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#  define CACHE_LINE_ALIGNMENT __declspec(align(CACHE_LINE_SIZE))
#else
#  define CACHE_LINE_ALIGNMENT  __attribute__ ((aligned(CACHE_LINE_SIZE)))
#endif

enum Color {
	WHITE, BLACK, NO_COLOR, COLOR_NB = 2
};

enum PieceType {
	NO_PIECE_TYPE, PAWN, BISHOP, ADVISOR, KNIGHT, CANNON, ROOK, KING,
	ALL_PIECES = 0,
	PIECE_TYPE_NB = 8
};

enum Piece {
	NO_PIECE,
	W_PAWN = 1, W_BISHOP, W_ADVISOR, W_KNIGHT, W_CANNON, W_ROOK, W_KING,
	B_PAWN = 9, B_BISHOP, B_ADVISOR, B_KNIGHT, B_CANNON, B_ROOK, B_KING,
	PIECE_NB = 16
};

enum KnightBlock {
	KNIGHT_LEG,
	KNIGHT_EYE
};

enum Square {
	SQ_A0, SQ_B0, SQ_C0, SQ_D0, SQ_E0, SQ_F0, SQ_G0, SQ_H0, SQ_I0,
	SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1, SQ_I1,
	SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2, SQ_I2,
	SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3, SQ_I3,
	SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4, SQ_I4,
	SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5, SQ_I5,
	SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6, SQ_I6,
	SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7, SQ_I7,
	SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8, SQ_I8,
	SQ_A9, SQ_B9, SQ_C9, SQ_D9, SQ_E9, SQ_F9, SQ_G9, SQ_H9, SQ_I9,
	SQ_NONE,

	SQUARE_NB = 90,

	DELTA_N =  9,
	DELTA_E =  1,
	DELTA_S = -9,
	DELTA_W = -1,

	DELTA_NN = DELTA_N + DELTA_N,
	DELTA_NE = DELTA_N + DELTA_E,
	DELTA_SE = DELTA_S + DELTA_E,
	DELTA_SS = DELTA_S + DELTA_S,
	DELTA_SW = DELTA_S + DELTA_W,
	DELTA_NW = DELTA_N + DELTA_W,

	DELTA_EE = DELTA_E + DELTA_E,
	DELTA_WW = DELTA_W + DELTA_W,

	DELTA_NNW= DELTA_NN + DELTA_W,
	DELTA_NNE= DELTA_NN + DELTA_E,
	DELTA_EEN= DELTA_EE + DELTA_N,
	DELTA_EES= DELTA_EE + DELTA_S,
	DELTA_SSE= DELTA_SS + DELTA_E,
	DELTA_SSW= DELTA_SS + DELTA_W,
	DELTA_WWN= DELTA_WW + DELTA_N,
	DELTA_WWS= DELTA_WW + DELTA_S,

	DELTA_NNWW= DELTA_NN + DELTA_WW,
	DELTA_NNEE= DELTA_NN + DELTA_EE,
	DELTA_SSEE= DELTA_SS + DELTA_EE,
	DELTA_SSWW= DELTA_SS + DELTA_WW,
	
};

enum File {
	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_I, FILE_NB
};

enum Rank {
	RANK_0, RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_NB
};

//Vertical flip
const int32_t VerticalFlip[SQUARE_NB] =
{ 
	SQ_A9, SQ_B9, SQ_C9, SQ_D9, SQ_E9, SQ_F9, SQ_G9, SQ_H9, SQ_I9,
	SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8, SQ_I8,
	SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7, SQ_I7,
	SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6, SQ_I6,
	SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5, SQ_I5,
	SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4, SQ_I4,
	SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3, SQ_I3,
	SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2, SQ_I2,
	SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1, SQ_I1,
	SQ_A0, SQ_B0, SQ_C0, SQ_D0, SQ_E0, SQ_F0, SQ_G0, SQ_H0, SQ_I0,
};
//Horizontal flip
const int32_t HorizontalFlip[SQUARE_NB] =
{ 
	SQ_I0, SQ_H0, SQ_G0, SQ_F0, SQ_E0, SQ_D0, SQ_C0, SQ_B0, SQ_A0,
	SQ_I1, SQ_H1, SQ_G1, SQ_F1, SQ_E1, SQ_D1, SQ_C1, SQ_B1, SQ_A1,
	SQ_I2, SQ_H2, SQ_G2, SQ_F2, SQ_E2, SQ_D2, SQ_C2, SQ_B2, SQ_A2,
	SQ_I3, SQ_H3, SQ_G3, SQ_F3, SQ_E3, SQ_D3, SQ_C3, SQ_B3, SQ_A3,
	SQ_I4, SQ_H4, SQ_G4, SQ_F4, SQ_E4, SQ_D4, SQ_C4, SQ_B4, SQ_A4,
	SQ_I5, SQ_H5, SQ_G5, SQ_F5, SQ_E5, SQ_D5, SQ_C5, SQ_B5, SQ_A5,
	SQ_I6, SQ_H6, SQ_G6, SQ_F6, SQ_E6, SQ_D6, SQ_C6, SQ_B6, SQ_A6,
	SQ_I7, SQ_H7, SQ_G7, SQ_F7, SQ_E7, SQ_D7, SQ_C7, SQ_B7, SQ_A7,
	SQ_I8, SQ_H8, SQ_G8, SQ_F8, SQ_E8, SQ_D8, SQ_C8, SQ_B8, SQ_A8,
	SQ_I9, SQ_H9, SQ_G9, SQ_F9, SQ_E9, SQ_D9, SQ_C9, SQ_B9, SQ_A9,
};

//SQUARE
const int32_t SquareMake[RANK_NB][FILE_NB] = 
{
	SQ_A0, SQ_B0, SQ_C0, SQ_D0, SQ_E0, SQ_F0, SQ_G0, SQ_H0, SQ_I0,
	SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1, SQ_I1,
	SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2, SQ_I2,
	SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3, SQ_I3,
	SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4, SQ_I4,
	SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5, SQ_I5,
	SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6, SQ_I6,
	SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7, SQ_I7,
	SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8, SQ_I8,
	SQ_A9, SQ_B9, SQ_C9, SQ_D9, SQ_E9, SQ_F9, SQ_G9, SQ_H9, SQ_I9,
};

//SQUARE_FILE
const int32_t SquareFile[SQUARE_NB] = 
{
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
};
//SQUARE_RANK
const int32_t SquareRank[SQUARE_NB] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7, 7,
	8, 8, 8, 8, 8, 8, 8, 8, 8,
	9, 9, 9, 9, 9, 9, 9, 9, 9,
};

#define ENABLE_SAFE_OPERATORS_ON(T)                                         \
	inline T operator+(const T d1, const T d2) { return T(int(d1) + int(d2)); } \
	inline T operator-(const T d1, const T d2) { return T(int(d1) - int(d2)); } \
	inline T operator*(int i, const T d) { return T(i * int(d)); }              \
	inline T operator*(const T d, int i) { return T(int(d) * i); }              \
	inline T operator-(const T d) { return T(-int(d)); }                        \
	inline T& operator+=(T& d1, const T d2) { return d1 = d1 + d2; }            \
	inline T& operator-=(T& d1, const T d2) { return d1 = d1 - d2; }            \
	inline T& operator*=(T& d, int i) { return d = T(int(d) * i); }

#define ENABLE_OPERATORS_ON(T) ENABLE_SAFE_OPERATORS_ON(T)                  \
	inline T& operator++(T& d) { return d = T(int(d) + 1); }                    \
	inline T& operator--(T& d) { return d = T(int(d) - 1); }                    \
	inline T operator/(const T d, int i) { return T(int(d) / i); }              \
	inline T& operator/=(T& d, int i) { return d = T(int(d) / i); }


ENABLE_OPERATORS_ON(PieceType)
ENABLE_OPERATORS_ON(Piece)
ENABLE_OPERATORS_ON(Color)
ENABLE_OPERATORS_ON(Square)
ENABLE_OPERATORS_ON(File)
ENABLE_OPERATORS_ON(Rank)

inline Color operator ~(Color c) {
	return Color(c ^ 1);
}

inline Square operator|(File f, Rank r) {
	return Square(SquareMake[r][f]);
}

inline File file_of(Square s) {
	return File(SquareFile[s]);
}

inline Rank rank_of(Square s) {
	return Rank(SquareRank[s]);
}

inline bool is_ok(Square s) {
	return s >= SQ_A0 && s <= SQ_I9;
}

inline bool is_ok(int s){
	return s >= SQ_A0 && s <= SQ_I9;
}

inline Color square_color(Square s){
	return s > SQ_I4 ? BLACK : WHITE;
}

inline Piece make_piece(Color c, PieceType pt) {
	return Piece((c << 3) | pt);
}

inline PieceType type_of(Piece p)  {
	return PieceType(p & 7);
}

inline Color color_of(Piece p) {
	assert(p != NO_PIECE);
	return Color(p >> 3);
}

inline char file_to_char(File f, bool tolower = true) {
	return char(f - FILE_A + (tolower ? 'a' : 'A'));
}

inline char rank_to_char(Rank r) {
	return char(r - RANK_0 + '0');
}
#include <string>
inline const std::string square_to_string(Square s) {
	char ch[] = { file_to_char(file_of(s)), rank_to_char(rank_of(s)), 0 };
	return ch;
}

#endif