#ifndef DATA_H
#define DATA_H
#include "bitboard.h"
#include "types.h"
#include "checksum.h"

#define POW_2(x)   ( (Bitboard(1, 0)<<(x)) )

const Bitboard  A0 = POW_2(SQ_A0), B0 = POW_2(SQ_B0), C0 = POW_2(SQ_C0), D0 = POW_2(SQ_D0), E0 = POW_2(SQ_E0), F0 = POW_2(SQ_F0), G0 = POW_2(SQ_G0), H0 = POW_2(SQ_H0), I0 = POW_2(SQ_I0),
A1 = POW_2(SQ_A1), B1 = POW_2(SQ_B1), C1 = POW_2(SQ_C1), D1 = POW_2(SQ_D1), E1 = POW_2(SQ_E1), F1 = POW_2(SQ_F1), G1 = POW_2(SQ_G1), H1 = POW_2(SQ_H1), I1 = POW_2(SQ_I1),
A2 = POW_2(SQ_A2), B2 = POW_2(SQ_B2), C2 = POW_2(SQ_C2), D2 = POW_2(SQ_D2), E2 = POW_2(SQ_E2), F2 = POW_2(SQ_F2), G2 = POW_2(SQ_G2), H2 = POW_2(SQ_H2), I2 = POW_2(SQ_I2),   
A3 = POW_2(SQ_A3), B3 = POW_2(SQ_B3), C3 = POW_2(SQ_C3), D3 = POW_2(SQ_D3), E3 = POW_2(SQ_E3), F3 = POW_2(SQ_F3), G3 = POW_2(SQ_G3), H3 = POW_2(SQ_H3), I3 = POW_2(SQ_I3),  
A4 = POW_2(SQ_A4), B4 = POW_2(SQ_B4), C4 = POW_2(SQ_C4), D4 = POW_2(SQ_D4), E4 = POW_2(SQ_E4), F4 = POW_2(SQ_F4), G4 = POW_2(SQ_G4), H4 = POW_2(SQ_H4), I4 = POW_2(SQ_I4), 
A5 = POW_2(SQ_A5), B5 = POW_2(SQ_B5), C5 = POW_2(SQ_C5), D5 = POW_2(SQ_D5), E5 = POW_2(SQ_E5), F5 = POW_2(SQ_F5), G5 = POW_2(SQ_G5), H5 = POW_2(SQ_H5), I5 = POW_2(SQ_I5), 
A6 = POW_2(SQ_A6), B6 = POW_2(SQ_B6), C6 = POW_2(SQ_C6), D6 = POW_2(SQ_D6), E6 = POW_2(SQ_E6), F6 = POW_2(SQ_F6), G6 = POW_2(SQ_G6), H6 = POW_2(SQ_H6), I6 = POW_2(SQ_I6),  
A7 = POW_2(SQ_A7), B7 = POW_2(SQ_B7), C7 = POW_2(SQ_C7), D7 = POW_2(SQ_D7), E7 = POW_2(SQ_E7), F7 = POW_2(SQ_F7), G7 = POW_2(SQ_G7), H7 = POW_2(SQ_H7), I7 = POW_2(SQ_I7),
A8 = POW_2(SQ_A8), B8 = POW_2(SQ_B8), C8 = POW_2(SQ_C8), D8 = POW_2(SQ_D8), E8 = POW_2(SQ_E8), F8 = POW_2(SQ_F8), G8 = POW_2(SQ_G8), H8 = POW_2(SQ_H8), I8 = POW_2(SQ_I8),
A9 = POW_2(SQ_A9), B9 = POW_2(SQ_B9), C9 = POW_2(SQ_C9), D9 = POW_2(SQ_D9), E9 = POW_2(SQ_E9), F9 = POW_2(SQ_F9), G9 = POW_2(SQ_G9), H9 = POW_2(SQ_H9), I9 = POW_2(SQ_I9);

const Bitboard FileABB = A0|A1|A2|A3|A4|A5|A6|A7|A8|A9;
const Bitboard FileBBB = B0|B1|B2|B3|B4|B5|B6|B7|B8|B9;
const Bitboard FileCBB = C0|C1|C2|C3|C4|C5|C6|C7|C8|C9;
const Bitboard FileDBB = D0|D1|D2|D3|D4|D5|D6|D7|D8|D9;
const Bitboard FileEBB = E0|E1|E2|E3|E4|E5|E6|E7|E8|E9;
const Bitboard FileFBB = F0|F1|F2|F3|F4|F5|F6|F7|F8|F9;
const Bitboard FileGBB = G0|G1|G2|G3|G4|G5|G6|G7|G8|G9;
const Bitboard FileHBB = H0|H1|H2|H3|H4|H5|H6|H7|H8|H9;
const Bitboard FileIBB = I0|I1|I2|I3|I4|I5|I6|I7|I8|I9;

const Bitboard Rank0BB = A0|B0|C0|D0|E0|F0|G0|H0|I0;
const Bitboard Rank1BB = A1|B1|C1|D1|E1|F1|G1|H1|I1;
const Bitboard Rank2BB = A2|B2|C2|D2|E2|F2|G2|H2|I2;
const Bitboard Rank3BB = A3|B3|C3|D3|E3|F3|G3|H3|I3;
const Bitboard Rank4BB = A4|B4|C4|D4|E4|F4|G4|H4|I4;
const Bitboard Rank5BB = A5|B5|C5|D5|E5|F5|G5|H5|I5;
const Bitboard Rank6BB = A6|B6|C6|D6|E6|F6|G6|H6|I6;
const Bitboard Rank7BB = A7|B7|C7|D7|E7|F7|G7|H7|I7;
const Bitboard Rank8BB = A8|B8|C8|D8|E8|F8|G8|H8|I8;
const Bitboard Rank9BB = A9|B9|C9|D9|E9|F9|G9|H9|I9;

const Bitboard WhiteCityBB = D0|E0|F0|D1|E1|F1|D2|E2|F2;
const Bitboard BlackCityBB = D7|E7|F7|D8|E8|F8|D9|E9|F9;

const Bitboard WhiteAdvisorCityBB = D0|F0|E1|D2|F2;
const Bitboard BlackAdvisorCityBB = D7|F7|E8|D9|F9;
const Bitboard WhiteBishopCityBB  = C0|G0|A2|E2|I2|C4|G4;
const Bitboard BlackBishopCityBB  = C9|G9|A7|E7|I7|C5|G5;

const Bitboard WhitePawnMaskBB = Rank9BB|Rank8BB|Rank7BB|Rank6BB|Rank5BB|A3|A4|C3|C4|E3|E4|G3|G4|I3|I4;
const Bitboard BlackPawnMaskBB = Rank0BB|Rank1BB|Rank2BB|Rank3BB|Rank4BB|A5|A6|C5|C6|E5|E6|G5|G6|I5|I6;

const Bitboard DarkSquares(0x0000000000000000, 0x1FFFFFFFFFFF);

CACHE_LINE_ALIGNMENT

extern Bitboard SquareBB[SQUARE_NB];
extern Bitboard SquareBBL90[SQUARE_NB];
extern Bitboard FileBB[FILE_NB];
extern Bitboard RankBB[RANK_NB];

extern Bitboard CityBB[COLOR_NB];
extern Bitboard AdvisorCityBB[COLOR_NB];
extern Bitboard BishopCityBB[COLOR_NB];

extern Bitboard RookAttackMask[SQUARE_NB];
extern Bitboard RookAttackToR0[SQUARE_NB][128];
extern Bitboard RookAttackToRL90[SQUARE_NB][256];

extern Bitboard KnightAttackMask[SQUARE_NB];
extern Bitboard KnightAttackTo[SQUARE_NB][256];
extern Bitboard KnightLeg[SQUARE_NB];
extern Bitboard KnightLegRL90[SQUARE_NB];
extern Bitboard KnightAttackFrom[SQUARE_NB][256];
extern Bitboard KnightEye[SQUARE_NB];
extern Bitboard KnightEyeRL90[SQUARE_NB];

extern Bitboard CannonAttackToR0[SQUARE_NB][128];
extern Bitboard CannonAttackToRL90[SQUARE_NB][256];
extern Bitboard CannonSuperR0[SQUARE_NB][128];
extern Bitboard CannonSuperRL90[SQUARE_NB][256];

extern Bitboard BishopAttackTo[SQUARE_NB][256];
extern Bitboard BishopLeg[SQUARE_NB];
extern Bitboard BishopLegRL90[SQUARE_NB];

extern Bitboard AdvisorAttackTo[SQUARE_NB];

extern Bitboard KingAttackTo[SQUARE_NB];

extern Bitboard PawnAttackTo[COLOR_NB][SQUARE_NB];
extern Bitboard PawnAttackFrom[COLOR_NB][SQUARE_NB];
extern Bitboard PawnMask[COLOR_NB];
extern Bitboard PassedRiverBB[COLOR_NB];

extern int SquareDistance[SQUARE_NB][SQUARE_NB];

extern int    MS1BTable[256];
extern Square BSFTable[128];

extern const uint64_t DeBruijn_64;
extern const uint32_t DeBruijn_32;

extern Bitboard  RMasks[SQUARE_NB];
extern Bitboard* RAttacks[SQUARE_NB];
extern Bitboard  RMagics[SQUARE_NB];
extern unsigned  RShifts[SQUARE_NB];

extern Bitboard  CannonMasks[SQUARE_NB];
extern Bitboard* CannonAttacks[SQUARE_NB];
extern Bitboard  CannonMagics[SQUARE_NB];
extern unsigned  CannonShifts[SQUARE_NB];

extern Bitboard BetweenBB[SQUARE_NB][SQUARE_NB];

inline Bitboard between_bb(Square s1, Square s2) {
	return BetweenBB[s1][s2];
}

inline int square_distance(Square s1, Square s2) {
	return SquareDistance[s1][s2];
}

inline int file_distance(Square s1, Square s2) {
	return abs(file_of(s1) - file_of(s2));
}

inline int rank_distance(Square s1, Square s2) {
	return abs(rank_of(s1) - rank_of(s2));
}

inline bool square_same_color(Square s1, Square s2){
    return square_color(s1) == square_color(s2);
}

inline Bitboard rank_bb(Rank r) {
	return RankBB[r];
}

inline Bitboard rank_bb(Square s) {
	return RankBB[rank_of(s)];
}

inline Bitboard file_bb(File f) {
	return FileBB[f];
}

inline Bitboard file_bb(Square s) {
	return FileBB[file_of(s)];
}

inline Bitboard operator&(const Bitboard& b, Square s) {
	return b & SquareBB[s];
}

inline Bitboard& operator&=(Bitboard& b, Square s) {
	return b &= SquareBB[s];
}

inline Bitboard operator|(const Bitboard& b, Square s) {
	return b | SquareBB[s];
}
inline Bitboard& operator|=(Bitboard& b, Square s) {
	return b |= SquareBB[s];
}

inline Bitboard operator^(const Bitboard& b, Square s) {
	return b ^ SquareBB[s];
}
inline Bitboard& operator^=(Bitboard& b, Square s) {
	return b ^= SquareBB[s];
}


inline bool bitboard_and_square(const Bitboard& b, Square s){
    
	return (b.bb[0]&SquareBB[s].bb[0])||(b.bb[1]&SquareBB[s].bb[1]);
}
inline bool bitboard_and_bitboard(const Bitboard& a,const Bitboard& b){
	return (a.bb[0]&b.bb[0])||(a.bb[1]&b.bb[1]);
}

//inline Bitboard operator&(Bitboard b, Square s) {
//	return b & SquareBB[s];
//}
//
//inline Bitboard& operator|=(Bitboard& b, Square s) {
//	return b |= SquareBB[s];
//}
//
//inline Bitboard& operator^=(Bitboard& b, Square s) {
//	return b ^= SquareBB[s];
//}
//
//inline Bitboard operator|(Bitboard b, Square s) {
//	return b | SquareBB[s];
//}
//
//inline Bitboard operator^(Bitboard b, Square s) {
//	return b ^ SquareBB[s];
//}

inline bool square_in_city(Square sq){
	return CityBB[square_color(sq)]&sq;
}

inline bool bishop_in_city(Square sq){
	return BishopCityBB[square_color(sq)]&sq;
}

inline bool advisor_in_city(Square sq){
	return AdvisorCityBB[square_color(sq)]&sq;
}

inline Bitboard rook_attacks_bb(Square s, Bitboard occ, Bitboard occl90)
{
	return (RookAttackToR0[s][((occ>>(rank_of(s)*9 /*rank_mult(rank_of(s))*/+ 1)).bb[0])&127]) | (RookAttackToRL90[s][((occl90>>(file_of(s)*10/*file_multi(file_of(s))*/ + 1)).bb[0])&255]);
}
inline Bitboard cannon_control_bb(Square s, Bitboard occ, Bitboard occl90)
{
	return (CannonAttackToR0[s][((occ>>(rank_of(s)*9/*rank_mult(rank_of(s))*/ + 1)).bb[0])&127])|(CannonAttackToRL90[s][((occl90>>(file_of(s)*10/*file_multi(file_of(s))*/ + 1)).bb[0])&255]);
}
inline Bitboard cannon_super_control_bb(Square s, Bitboard occ, Bitboard occl90)
{
	return (CannonSuperR0[s][((occ >> (rank_of(s) * 9/*rank_mult(rank_of(s))*/ + 1)).bb[0]) & 127]) | (CannonSuperRL90[s][((occl90 >> (file_of(s) * 10/*file_multi(file_of(s))*/ + 1)).bb[0]) & 255]);
}
inline Bitboard knight_attacks_bb(Square s, Bitboard occ, Bitboard occl90)
{
	return KnightAttackTo[s][checksum(occl90&KnightLegRL90[s])];//马腿的旋转位棋盘进行折叠计算
}
inline Bitboard knight_attacks_from(Square s, Bitboard occ, Bitboard occl90)
{
    return KnightAttackFrom[s][checksum(occl90&KnightEyeRL90[s])];
}
inline Bitboard bishop_attacks_bb(Square s,  Bitboard occ, Bitboard occl90)
{
    return BishopAttackTo[s][checksum(occl90&BishopLegRL90[s])];
}
inline Bitboard pawn_attacks_from(Color opp, Square s)
{
    return PawnAttackFrom[opp][s];
}
inline Bitboard pawn_attacks_bb(Color c, Square s)
{
	return PawnAttackTo[c][s];
}
inline Bitboard shift_bb(Bitboard b,Square Delta) {

	return Delta == DELTA_N ? b << 9 : Delta == DELTA_S ? b >> 9
		:  Delta == DELTA_W ? (b & ~FileABB) >> 1 : Delta == DELTA_E ? (b & ~FileIBB) << 1
		:  Bitboard();
}

/// Functions for computing sliding attack bitboards. Function attacks_bb() takes
/// a square and a bitboard of occupied squares as input, and returns a bitboard
/// representing all squares attacked by Pt (bishop or rook) on the given square.
template<PieceType Pt>
inline unsigned magic_index(Square s, Bitboard occ) {

	Bitboard* const Masks  = Pt == ROOK ? RMasks  : CannonMasks;
	Bitboard* const Magics = Pt == ROOK ? RMagics : CannonMagics;
	unsigned* const Shifts = Pt == ROOK ? RShifts : CannonShifts;

	//if (HasPext)
	//	return unsigned(_pext_u64(occ, Masks[s]));

	//if (Is64Bit)
	//	return unsigned(((occ & Masks[s]) * Magics[s]) >> Shifts[s]);

	//unsigned lo = unsigned(occ) & unsigned(Masks[s]);
	//unsigned hi = unsigned(occ >> 32) & unsigned(Masks[s] >> 32);
	//return (lo * unsigned(Magics[s]) ^ hi * unsigned(Magics[s] >> 32)) >> Shifts[s];


   uint64_t bb[2];
   bb[0] = occ.bb[0]&Masks[s].bb[0];
   bb[1] = occ.bb[1]&Masks[s].bb[1];

   //very important, must << 19, be sure bb[i]*Magics[s].bb[i] hight bit is no zero
   return  (( (bb[0]*Magics[s].bb[0])<<19 ) ^ ( (bb[1]*Magics[s].bb[1]))<<19 ) >> Shifts[s];
}

extern Square msb(Bitboard b);
extern Square lsb( Bitboard b);
extern Square pop_lsb(Bitboard* b);
extern uint32_t pop_count(const Bitboard& b);

extern void init_data();
#endif