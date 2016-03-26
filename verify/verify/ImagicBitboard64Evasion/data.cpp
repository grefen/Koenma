#include <algorithm>
#include <stdlib.h>
#include "data.h"
#include "rkiss.h"
#include "bitcount.h"
#include <fstream>
#include "imagics.h"

CACHE_LINE_ALIGNMENT

Bitboard SquareBB[SQUARE_NB];
Bitboard FileBB[FILE_NB];
Bitboard RankBB[RANK_NB];

Bitboard CityBB[COLOR_NB];
Bitboard AdvisorCityBB[COLOR_NB];
Bitboard BishopCityBB[COLOR_NB];

Bitboard RookTable[1081344];//1081344//1056k*8 mem, very big
Bitboard CannonTable[1081344];
Bitboard SuperCannonTable[1081344];

Bitboard  RookAttackMask[SQUARE_NB];
Bitboard  RMasks[SQUARE_NB];
Bitboard* RAttacks[SQUARE_NB];
Bitboard  RMagics[SQUARE_NB];
unsigned  RShifts[SQUARE_NB];

Bitboard  CannonMasks[SQUARE_NB];
Bitboard* CannonAttacks[SQUARE_NB];
Bitboard  CannonMagics[SQUARE_NB];
unsigned  CannonShifts[SQUARE_NB];

Bitboard  SuperCannonMasks[SQUARE_NB];
Bitboard* SuperCannonAttacks[SQUARE_NB];
Bitboard  SuperCannonMagics[SQUARE_NB];
unsigned  SuperCannonShifts[SQUARE_NB];

Bitboard KnightAttackMask[SQUARE_NB];
Bitboard KnightLeg[SQUARE_NB];
Bitboard KnightEye[SQUARE_NB];
Bitboard KnightImagicAttackTo[SQUARE_NB][16];
Bitboard KnightImagicAttackFrom[SQUARE_NB][16];
uint64_t KnightLegImagic[SQUARE_NB];
uint64_t KnightEyeImagic[SQUARE_NB];


Bitboard BishopLeg[SQUARE_NB];
Bitboard BishopImagicAttack[SQUARE_NB][16];
uint64_t BishopLegImagic[SQUARE_NB];


Bitboard AdvisorAttackTo[SQUARE_NB];

Bitboard KingAttackTo[SQUARE_NB];

Bitboard PawnAttackTo[COLOR_NB][SQUARE_NB];
Bitboard PawnAttackFrom[COLOR_NB][SQUARE_NB];
Bitboard PawnMask[COLOR_NB];
Bitboard PassedRiverBB[COLOR_NB];

int SquareDistance[SQUARE_NB][SQUARE_NB];

int    MS1BTable[256];
Square BSFTable[128];

Bitboard BetweenBB[SQUARE_NB][SQUARE_NB];

const uint64_t DeBruijn_64 = 0x3F79D71B4CB0A89ULL;
const uint32_t DeBruijn_32 = 0x783A9B23;

typedef unsigned (IndexFn)(Square sq, const Bitboard& occupied);
typedef Bitboard (AttackFun)(Square deltas[], Square sq, const Bitboard& occupied);
typedef Bitboard (KAttackFun)(Square sq, const Bitboard& occupied);

void init_slider_magics(Bitboard table[],
	Bitboard* attacks[],
	Bitboard magics[],
	Bitboard masks[],
	unsigned shifts[],
	Square deltas[],
	IndexFn index, 
	AttackFun attackfun);

void init_knight_maigcs(Bitboard attack[SQUARE_NB][16],
	Bitboard legoreye[SQUARE_NB],
	uint64_t imagic[SQUARE_NB],
	IndexFn index,
	KAttackFun attackfun
	);
void init_bishop_magics();

void init_slider_magics(Bitboard table[],
	Bitboard* attacks[],
	Bitboard magics[],
	Bitboard masks[],
	unsigned shifts[],
	Square deltas[],
	IndexFn index,
	AttackFun attackfun,
	Bitboard magicsdata[],
	unsigned shiftsdata[]);

void init_knight_maigcs(Bitboard attack[SQUARE_NB][16],
	Bitboard legoreye[SQUARE_NB],
	uint64_t imagic[SQUARE_NB],
	IndexFn index,
	KAttackFun attackfun,
	uint64_t magicsdata[]
	);

void init_bishop_magics(uint64_t magicsdata[]);

uint32_t pop_count(const Bitboard& b)
{
	return popcount<CNT_64>(b.bb[0]) + popcount<CNT_64>(b.bb[1]);
}

Bitboard sliding_attack(Square deltas[], Square sq, const Bitboard& occupied) {

	Bitboard attack(0, 0);

	//square_distance作用是防止边界发生回环，如最右边的sq加1会回环到最左上位置
	for (int i = 0; i < 4; ++i)
		for (Square s = sq + deltas[i];
	is_ok(s) && square_distance(s, s - deltas[i]) == 1;
		s += deltas[i])
	{
		attack |= s;

		if (occupied & s)
			break;
	}

	return attack;
}

Bitboard cannon_sliding_control(Square deltas[], Square sq, const Bitboard& occupied) {

	Bitboard attack(0, 0);

	//square_distance作用是防止边界发生回环，如最右边的sq加1会回环到最左上位置
	for (int i = 0; i < 4; ++i) {
		int count = 0;
		for (Square s = sq + deltas[i];	is_ok(s) && square_distance(s, s - deltas[i]) == 1;
		s += deltas[i])
		{
			if (count == 1)
			{
				attack |= s;
			}
			else if (count >= 2)
			{
				break;
			}

			if (occupied & s)
			{
				count++;
			}
		}
	}

	return attack;
}
Bitboard supercannon_sliding_control(Square deltas[], Square sq, const Bitboard& occupied) {

	Bitboard attack(0, 0);

	//square_distance作用是防止边界发生回环，如最右边的sq加1会回环到最左上位置
	for (int i = 0; i < 4; ++i) {
		int count = 0;
		for (Square s = sq + deltas[i];	is_ok(s) && square_distance(s, s - deltas[i]) == 1;
		s += deltas[i])
		{
			if (count == 1)
			{
				//attack |= s;
			}
			else if (count == 2)
			{
				attack |= s;
			}
			else if (count >= 3)
			{
				break;
			}

			if (occupied & s)
			{
				count++;
			}
		}
	}

	return attack;
}
Bitboard knight_attack_to(Square sq, const Bitboard& leg) {

	Bitboard attack;

	Square KnightLegDeltas[4] = { DELTA_N, DELTA_E,DELTA_S,DELTA_W };
	Square KnightStep[4][2] = { { DELTA_NNW,DELTA_NNE },{ DELTA_EEN, DELTA_EES },{ DELTA_SSE, DELTA_SSW },{ DELTA_WWS,DELTA_WWN } };

	for (int dir = 0; dir < 4; ++dir) {
		for (int foot = 0; foot < 2; ++foot) {
			int to = (int)sq + (int)KnightStep[dir][foot];//马脚
			if (is_ok(to) && square_distance(Square(sq), Square(to)) < 3) {	//在棋盘上，并且没有发生回旋
				int legsq = int(sq) + KnightLegDeltas[dir];//马腿
				if (is_ok(legsq) && square_distance(Square(sq), Square(legsq)) == 1) { // 在棋盘上，并且没有发生回旋
					if (!(leg & SquareBB[legsq])) { //不存在马腿
						attack |= SquareBB[to];
					}
				}
			}
		}
	}

	return attack;
}

Bitboard knight_attack_from(Square sq, const Bitboard& eye) {

	Bitboard attack;

	Square KnightEyeDeltas[4] = { DELTA_NW, DELTA_NE, DELTA_SE, DELTA_SW };
	Square KnightStepFrom[4][2] = { { DELTA_WWN,DELTA_NNW },{ DELTA_NNE,DELTA_EEN },{ DELTA_EES,DELTA_SSE },{ DELTA_SSW, DELTA_WWS } };

	for (int dir = 0; dir < 4; ++dir) {	 //四个方向
		for (int fr = 0; fr < 2; ++fr) {	//每个方向两个攻击位置
			int from = (int)sq + (int)KnightStepFrom[dir][fr];
			if (is_ok(from) && square_distance(Square(sq), Square(from)) < 3) {	//在棋盘上，且没有发生回旋
				int sqeye = int(sq) + int(KnightEyeDeltas[dir]);
				if (is_ok(sqeye) && square_distance(Square(sq), Square(sqeye)) == 1) {	 //在棋盘上，且没有发生回旋
					if (!(eye&SquareBB[sqeye])) { //马眼位置不存在子
						attack |= SquareBB[from];
					}
				}
			}
		}
	}

	return attack;
}

Bitboard bishop_attack_to(Square sq, const Bitboard& eye) {
	Bitboard attack;

	Square EyeDeltas[4] = { DELTA_NW, DELTA_NE, DELTA_SE, DELTA_SW };
	Square Step[4] = { DELTA_NNWW, DELTA_NNEE, DELTA_SSEE,DELTA_SSWW };

	for (int dir = 0; dir < 4; ++dir) {
		int to = (int)sq + (int)Step[dir];
		if (is_ok(to) && square_distance(Square(sq), Square(to)) < 3 && square_same_color(Square(sq), Square(to)) && bishop_in_city(Square(to)) && bishop_in_city(Square(sq))) {	 //在棋盘上，且没有发生回绕
			int bishopeye = int(sq) + int(EyeDeltas[dir]);
			if (is_ok(bishopeye) && square_distance(Square(sq), Square(bishopeye)) == 1 && square_same_color(Square(sq), Square(to))) {
				if (!(eye&SquareBB[bishopeye])) { //相眼位置不存在子
					attack |= SquareBB[to];
				}
			}
		}
	}

	return 	attack;

}
//
uint32_t bsf_index90(Bitboard b) {

	//64bit
	if (b.bb[0] > 0)
	{
		return (((b.bb[0] ^ (b.bb[0] - 1))* DeBruijn_64) >> 58);
	}
	else if (b.bb[1] > 0)
	{
		return (((b.bb[1] ^ (b.bb[1] - 1))* DeBruijn_64) >> 58) + 64;
	}

	return 0;
}

Square msb(Bitboard b) {

	uint64_t b64;
	int result = 0;

	if (b.bb[1] > 0)
	{
		result = 45;

		b64 = b.bb[1];

		if (b64 > 0xFFFFFFFF)
		{
			b64 >>= 32;
			result += 32;
		}

		if (b64 > 0xFFFF)
		{
			b64 >>= 16;
			result += 16;
		}

		if (b64 > 0xFF)
		{
			b64 >>= 8;
			result += 8;
		}

		return (Square)(result + MS1BTable[b64]);
	}
	else
	{
		b64 = b.bb[0];

		if (b64 > 0xFFFFFFFF)
		{
			b64 >>= 32;
			result += 32;
		}

		if (b64 > 0xFFFF)
		{
			b64 >>= 16;
			result += 16;
		}

		if (b64 > 0xFF)
		{
			b64 >>= 8;
			result += 8;
		}

		return (Square)(result + MS1BTable[b64]);
	}
}
Square lsb(Bitboard b)
{
	return BSFTable[bsf_index90(b)];
}
Square pop_lsb(Bitboard* b)
{
	Bitboard bb = *b;
	b->pop_lsb();
	return BSFTable[bsf_index90(bb)];
}

void init_data() {

	for (int k = 0, i = 0; i < 8; ++i)
		while (k < (2 << i))
			MS1BTable[k++] = i;

	for (int i = 0; i < SQUARE_NB; ++i)
	{
		BSFTable[bsf_index90(Bitboard(1, 0) << i)] = Square(i);
	}


	for (int i = 0; i < SQUARE_NB; ++i)
	{
		SquareBB[i] = (Bitboard(1, 0) << (i));
	}

	FileBB[FILE_A] = FileABB;
	FileBB[FILE_B] = FileBBB;
	FileBB[FILE_C] = FileCBB;
	FileBB[FILE_D] = FileDBB;
	FileBB[FILE_E] = FileEBB;
	FileBB[FILE_F] = FileFBB;
	FileBB[FILE_G] = FileGBB;
	FileBB[FILE_H] = FileHBB;
	FileBB[FILE_I] = FileIBB;

	RankBB[RANK_0] = Rank0BB;
	RankBB[RANK_1] = Rank1BB;
	RankBB[RANK_2] = Rank2BB;
	RankBB[RANK_3] = Rank3BB;
	RankBB[RANK_4] = Rank4BB;
	RankBB[RANK_5] = Rank5BB;
	RankBB[RANK_6] = Rank6BB;
	RankBB[RANK_7] = Rank7BB;
	RankBB[RANK_8] = Rank8BB;
	RankBB[RANK_9] = Rank9BB;

	CityBB[WHITE] = WhiteCityBB;
	CityBB[BLACK] = BlackCityBB;

	AdvisorCityBB[WHITE] = WhiteAdvisorCityBB;
	AdvisorCityBB[BLACK] = BlackAdvisorCityBB;

	BishopCityBB[WHITE] = WhiteBishopCityBB;
	BishopCityBB[BLACK] = BlackBishopCityBB;

	PawnMask[WHITE] = WhitePawnMaskBB;
	PawnMask[BLACK] = BlackPawnMaskBB;

	PassedRiverBB[WHITE] = DarkSquares;
	PassedRiverBB[BLACK] = ~DarkSquares;

	for (Square s1 = SQ_A0; s1 <= SQ_I9; ++s1)
	{
		for (Square s2 = SQ_A0; s2 <= SQ_I9; ++s2)
		{
			SquareDistance[s1][s2] = std::max(file_distance(s1, s2), rank_distance(s1, s2));
		}
	}

	Square RDeltas[] = { DELTA_N,  DELTA_E,  DELTA_S,  DELTA_W };
	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		RookAttackMask[s] = sliding_attack(RDeltas, s, Bitboard());
	}

	//knight leg
	//top east souse west
	Square KnightLegDeltas[4] = { DELTA_N, DELTA_E,DELTA_S,DELTA_W };
	Square KnightStep[4][2] = { {DELTA_NNW,DELTA_NNE},{DELTA_EEN, DELTA_EES},{DELTA_SSE, DELTA_SSW},{DELTA_WWS,DELTA_WWN} };
	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		KnightLeg[s] = Bitboard();
		for (int i = 0; i < 4; ++i)
		{
			int to = (int)s + (int)KnightLegDeltas[i];//枚举类型可能不存在，所以用int类型
			if (is_ok(to) && square_distance(Square(s), Square(to)) == 1)//在棋盘上，并且没有发生回旋
			{
				KnightLeg[s] |= SquareBB[to];
			}
		}

		for (int j = 0; j < 4; ++j)//四个方向
		{
			for (int k = 0; k < 2; ++k)//每个方向两个马脚
			{
				int to = int(s) + KnightStep[j][k];//马脚
				if (is_ok(to) && square_distance(Square(s), Square(to)) < 3)//在棋盘上，并且没有发生回旋
				{
					KnightAttackMask[s] |= SquareBB[to];//无需顾虑马腿问题
				}
			}
		}

	}

	//knight eye
	Square KnightEyeDeltas[4] = { DELTA_NW, DELTA_NE, DELTA_SE, DELTA_SW };
	Square KnightStepFrom[4][2] = { {DELTA_WWN,DELTA_NNW},{DELTA_NNE,DELTA_EEN},{DELTA_EES,DELTA_SSE},{DELTA_SSW, DELTA_WWS} };
	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		for (int i = 0; i < 4; ++i)
		{
			int to = (int)s + (int)KnightEyeDeltas[i];
			if (is_ok(to) && square_distance(Square(s), Square(to)) == 1)
			{
				KnightEye[s] |= SquareBB[to];
			}
		}
	}


	//bishop
	Square BishopEyeDeltas[4] = { DELTA_NW, DELTA_NE, DELTA_SE, DELTA_SW };
	Square BishopStepDeltas[4] = { DELTA_NNWW, DELTA_NNEE, DELTA_SSEE, DELTA_SSWW };
	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		BishopLeg[s] = Bitboard();
		for (int i = 0; i < 4; ++i)
		{
			int to = (int)s + (int)BishopEyeDeltas[i];
			if (is_ok(to) && square_distance(Square(s), Square(to)) == 1 && square_same_color(Square(s), Square(to)))
			{
				BishopLeg[s] |= SquareBB[to];
			}
		}
	}

	//advisor
	Square AdvisorDetas[4] = { DELTA_NW, DELTA_NE, DELTA_SE, DELTA_SW };
	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		AdvisorAttackTo[s] = Bitboard();
		for (int i = 0; i < 4; ++i)
		{
			int to = (int)s + AdvisorDetas[i];
			if (is_ok(to) && square_distance(Square(s), Square(to)) == 1 && advisor_in_city(Square(to)) && advisor_in_city(Square(s)))
			{
				AdvisorAttackTo[s] |= SquareBB[to];
			}
		}
	}

	//pawn
	Square PawnDetas[COLOR_NB][3] = { {DELTA_N,DELTA_W, DELTA_E},{DELTA_S,DELTA_E,DELTA_W} };
	for (Color c = WHITE; c < COLOR_NB; ++c)
	{
		for (Square s = SQ_A0; s <= SQ_I9; ++s)
		{

			PawnAttackTo[c][s] = Bitboard();

			for (int i = 0; i < 3; ++i)
			{
				int to = int(s) + PawnDetas[c][i];
				if (is_ok(to) && square_distance(Square(s), Square(to)) == 1)
				{
					if (PawnMask[c] & SquareBB[s])//攻击位置必须是正确的
					{
						if (PawnMask[c] & SquareBB[to])//mask是pawn的正确位置掩码，落在mask里面得必然是正确的
						{
							PawnAttackTo[c][s] |= SquareBB[to];
						}
					}

				}
			}

			Color opp = ~c;

			PawnAttackFrom[opp][s] = Bitboard();

			for (int i = 0; i < 3; ++i)
			{
				int to = int(s) + PawnDetas[c][i];//s位置按照我方走法，得到的是可以攻击到s的位置
				if (is_ok(to) && square_distance(Square(s), Square(to)) == 1)
				{
					if (PawnMask[opp] & SquareBB[s])//受攻击位置在opp的mask上，防止在对方河界上计算出错
					{
						if (PawnMask[opp] & SquareBB[to])//如果to在对方的mask里面，必定是对方合理的走法
						{
							PawnAttackFrom[opp][s] |= SquareBB[to];//s位置遭受对方攻击
						}
					}

				}
			}

		}
	}


	//king
	Square KingDetas[4] = { DELTA_N, DELTA_E, DELTA_S,DELTA_W };
	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		KingAttackTo[s] = Bitboard();
		for (int i = 0; i < 4; ++i)
		{
			int to = (int)s + KingDetas[i];
			if (is_ok(to) && square_distance(Square(s), Square(to)) == 1)
			{
				if (square_in_city(Square(to)) && square_in_city(Square(s)))
				{
					KingAttackTo[s] |= SquareBB[to];
				}
			}
		}

	}

	for (Square s1 = SQ_A0; s1 <= SQ_I9; ++s1)
		for (Square s2 = SQ_A0; s2 <= SQ_I9; ++s2)
			if (RookAttackMask[s1] & s2)
			{
				Square delta = (s2 - s1) / square_distance(s1, s2);

				for (Square s = s1 + delta; s != s2; s += delta)
					BetweenBB[s1][s2] |= s;
			}


	init_slider_magics(RookTable, RAttacks, RMagics, RMasks, RShifts, RDeltas, slider_magic_index<ROOK>, sliding_attack, RookMagicsData, RookShiftsData);
	init_slider_magics(CannonTable, CannonAttacks, CannonMagics, CannonMasks, CannonShifts, RDeltas, slider_magic_index<CANNON>, cannon_sliding_control, CannonMagicsData, CannonShiftsData);
	init_slider_magics(SuperCannonTable, SuperCannonAttacks, SuperCannonMagics, SuperCannonMasks, SuperCannonShifts, RDeltas, supercannon_magic_index, supercannon_sliding_control, SuperCannonMagicsData, SuperCannonShiftsData);

	init_knight_maigcs(KnightImagicAttackTo, KnightLeg, KnightLegImagic, knight_imagic_index<KNIGHT_LEG>, knight_attack_to, KnightLegMagicsData);
	init_knight_maigcs(KnightImagicAttackFrom, KnightEye, KnightEyeImagic, knight_imagic_index<KNIGHT_EYE>, knight_attack_from, KnightEyeMagicsData);

	init_bishop_magics(BishopLegMagicsData);

#if 0
	//init imagics
	init_slider_magics(RookTable, RAttacks, RMagics, RMasks, RShifts, RDeltas, slider_magic_index<ROOK>, sliding_attack);
	init_slider_magics(CannonTable, CannonAttacks, CannonMagics, CannonMasks, CannonShifts, RDeltas, slider_magic_index<CANNON>, cannon_sliding_control);
	init_slider_magics(SuperCannonTable, SuperCannonAttacks, SuperCannonMagics, SuperCannonMasks, SuperCannonShifts, RDeltas, supercannon_magic_index, supercannon_sliding_control);

	init_knight_maigcs(KnightImagicAttackTo,KnightLeg,KnightLegImagic,knight_imagic_index<KNIGHT_LEG>,knight_attack_to);
	init_knight_maigcs(KnightImagicAttackFrom,KnightEye,KnightEyeImagic,knight_imagic_index<KNIGHT_EYE>,knight_attack_from);

	init_bishop_magics();

    //write to text
	using namespace std;
	fstream fmagics("imagics.h", ios::trunc | ios::out);

	fmagics << "#ifndef IMAGICS_H\n";
	fmagics << "#define IMAGICS_H\n";
	fmagics << "#include\"platform.h\"\n";

	//rook
	fmagics << "Bitboard RookMagicsData[90]={\n"; 
	for (Square s = SQ_A0; s <= SQ_I9; ++s) {
		fmagics << showbase << hex << "{" << RMagics[s].bb[0] << ", " << RMagics[s].bb[1] << "}," << "\n";
	}
	fmagics << "};\n";

	fmagics << "unsigned RookShiftsData[90]={\n";
	for (Square s = SQ_A0; s <= SQ_I9; ++s) {
		fmagics << showbase << hex << RShifts[s]<< ",\n";
	}
	fmagics << "};\n";
	//cannon
	fmagics << "Bitboard CannonMagicsData[90]={\n";
	for (Square s = SQ_A0; s <= SQ_I9; ++s) {
		fmagics << showbase << hex << "{" << CannonMagics[s].bb[0] << ", " << CannonMagics[s].bb[1] << "}," << "\n";
	}
	fmagics << "};\n";

	fmagics << "unsigned CannonShiftsData[90]={\n";
	for (Square s = SQ_A0; s <= SQ_I9; ++s) {
		fmagics << showbase << hex << CannonShifts[s] << ",\n";
	}
	fmagics << "};\n";

	//supercannon
	fmagics << "Bitboard SuperCannonMagicsData[90]={\n";
	for (Square s = SQ_A0; s <= SQ_I9; ++s) {
		fmagics << showbase << hex << "{" << SuperCannonMagics[s].bb[0] << ", " << SuperCannonMagics[s].bb[1] << "}," << "\n";
	}
	fmagics << "};\n";

	fmagics << "unsigned SuperCannonShiftsData[90]={\n";
	for (Square s = SQ_A0; s <= SQ_I9; ++s) {
		fmagics << showbase << hex << SuperCannonShifts[s] << ",\n";
	}
	fmagics << "};\n";

	//knight
	fmagics << "uint64_t KnightLegMagicsData[90]={\n";
	for (Square s = SQ_A0; s <= SQ_I9; ++s) {
		fmagics << showbase << hex << KnightLegImagic[s] << ",\n";
	}
	fmagics << "};\n";

	fmagics << "uint64_t KnightEyeMagicsData[90]={\n";
	for (Square s = SQ_A0; s <= SQ_I9; ++s) {
		fmagics << showbase << hex << KnightEyeImagic[s] << ",\n";
	}
	fmagics << "};\n";

	//bishop
	fmagics << "uint64_t BishopLegMagicsData[90]={\n";
	for (Square s = SQ_A0; s <= SQ_I9; ++s) {
		fmagics << showbase << hex << BishopLegImagic[s] << ",\n";
	}
	fmagics << "};\n";

	fmagics << "#endif";

#endif
}

// init_magics() computes all rook and cannon attacks at startup. Magic
// bitboards are used to look up attacks of sliding pieces. As a reference see
// chessprogramming.wikispaces.com/Magic+Bitboards. In particular, here we
// use the so called "fancy" approach.
void init_slider_magics(Bitboard table[],
	                    Bitboard* attacks[],
	                    Bitboard magics[],	
	                    Bitboard masks[],
	                    unsigned shifts[],
	                    Square deltas[],
	                    IndexFn index, 
	                    AttackFun attackfun) {

	int MagicBoosters[][10] = { { 969, 1976, 2850,  969, 2069, 2852, 1708,  164, 164, 164 },
	{ 3101,  552, 3555,  926,  834,   26, 2131, 1117, 1117, 1117 } };

	RKISS rk;
	Bitboard occupancy[1 << 15], reference[1 << 15], edges, b;
	int i, size, booster;

	// attacks[s] is a pointer to the beginning of the attacks table for square 's'
	attacks[SQ_A0] = table;

	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		// Board edges are not considered in the relevant occupancies
		edges = ((Rank0BB | Rank9BB) & ~rank_bb(s)) | ((FileABB | FileIBB) & ~file_bb(s));

		// Given a square 's', the mask is the bitboard of sliding attacks from
		// 's' computed on an empty board. The index must be big enough to contain
		// all the attacks for each possible subset of the mask and so is 2 power
		// the number of 1s of the mask. Hence we deduce the size of the shift to
		// apply to the 64 or 32 bits word to get the index.
		masks[s] = sliding_attack(deltas, s, Bitboard()) & ~edges;
		shifts[s] = 64 - pop_count(masks[s]);

		// Use Carry-Rippler trick to enumerate all subsets of masks[s] and
		// store the corresponding sliding attack bitboard in reference[].

		size = 0;
		b = Bitboard();

		do
		{
			do
			{
				occupancy[size] = b;
				reference[size] = attackfun(deltas, s, b);

				size++;

				b.bb[0] = (b.bb[0] - masks[s].bb[0])&masks[s].bb[0];

			} while (b.bb[0]);
			b.bb[1] = (b.bb[1] - masks[s].bb[1])&masks[s].bb[1];

		} while (b.bb[1]);

		// Set the offset for the table of the next square. We have individual
		// table sizes for each square with "Fancy Magic Bitboards".
		if (s < SQ_I9)
			attacks[s + 1] = attacks[s] + size;

		booster = MagicBoosters[1][rank_of(s)];

		// Find a magic for square 's' picking up an (almost) random number
		// until we find the one that passes the verification test.

		do {

			do
			{
				magics[s] = rk.magic_bitboard(booster);

			} while (popcount<CNT_64>(((magics[s].bb[0] * masks[s].bb[0]) ^ (magics[s].bb[1] * masks[s].bb[1])) >> 54) < 5);


			std::memset(attacks[s], 0, size * sizeof(Bitboard));

			// A good magic must map every possible occupancy to an index that
			// looks up the correct sliding attack in the attacks[s] database.
			// Note that we build up the database for square 's' as a side
			// effect of verifying the magic.
			for (i = 0; i < size; ++i)
			{
				Bitboard& attack = attacks[s][index(s, occupancy[i])];

				if (attack && attack != reference[i])
					break;

				attack = reference[i];
			}

		} while (i < size);
		printf("sq:%d,bb0:0x%llX,bb1:0x%llX  size: %d\n", s, magics[s].bb[0], magics[s].bb[1], size);

		
	}

	
}

void init_knight_maigcs(Bitboard attacktable[SQUARE_NB][16],
	                    Bitboard legoreye[SQUARE_NB],
	                    uint64_t imagic[SQUARE_NB],
	                    IndexFn index,
	                    KAttackFun attackfun
	                   )
{

	int MagicBoosters[][10] = { { 969, 1976, 2850,  969, 2069, 2852, 1708,  164, 164, 164 },
	{ 3101,  552, 3555,  926,  834,   26, 2131, 1117, 1117, 1117 } };

	RKISS rk;
	Bitboard occupancy[1 << 4], reference[1 << 4], edges, b;
	int i, size, booster;


	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		size = 0;
		b = Bitboard();
		//遍历kinghteye的bit组成的所有情况
		do
		{
			do
			{
				occupancy[size] = b;
				reference[size] = attackfun(s, b);

				size++;

				b.bb[0] = (b.bb[0] - legoreye[s].bb[0])&legoreye[s].bb[0];

			} while (b.bb[0]);
			b.bb[1] = (b.bb[1] - legoreye[s].bb[1])&legoreye[s].bb[1];

		} while (b.bb[1]);

		booster = MagicBoosters[1][rank_of(s)];

		do {

			imagic[s] = rk.magic_rand<uint64_t>(booster);

			std::memset(attacktable[s], 0, size * sizeof(Bitboard));

			// A good magic must map every possible occupancy to an index that
			// looks up the correct sliding attack in the attacks[s] database.
			// Note that we build up the database for square 's' as a side
			// effect of verifying the magic.
			for (i = 0; i < size; ++i)
			{
				Bitboard& attack = attacktable[s][index(s, occupancy[i])];

				if (attack && attack != reference[i])
					break;

				attack = reference[i];
			}

		} while (i < size);

		printf("sq:%d,imagic:0x%llX,  size: %d\n", s, imagic[s], size);

		
	}
	
}

void init_bishop_magics() {
	int MagicBoosters[][10] = { { 969, 1976, 2850,  969, 2069, 2852, 1708,  164, 164, 164 },
	{ 3101,  552, 3555,  926,  834,   26, 2131, 1117, 1117, 1117 } };

	RKISS rk;
	Bitboard occupancy[1 << 4], reference[1 << 4], edges, b;
	int i, size, booster;


	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		//s不在相应该在的位置上
		if (!bishop_in_city(s)) {
			
			continue;
		}

		size = 0;
		b = Bitboard();
		//遍历eye的bit组成的所有情况
		do
		{
			do
			{
				occupancy[size] = b;
				reference[size] = bishop_attack_to(s, b);

				size++;

				b.bb[0] = (b.bb[0] - BishopLeg[s].bb[0])&BishopLeg[s].bb[0];

			} while (b.bb[0]);
			b.bb[1] = (b.bb[1] - BishopLeg[s].bb[1])&BishopLeg[s].bb[1];

		} while (b.bb[1]);

		booster = MagicBoosters[1][rank_of(s)];

		do {

			BishopLegImagic[s] = rk.magic_rand<uint64_t>(booster);

			std::memset(BishopImagicAttack[s], 0, size * sizeof(Bitboard));

			// A good magic must map every possible occupancy to an index that
			// looks up the correct sliding attack in the attacks[s] database.
			// Note that we build up the database for square 's' as a side
			// effect of verifying the magic.
			for (i = 0; i < size; ++i)
			{
				Bitboard& attack = BishopImagicAttack[s][bishop_imagic_index(s, occupancy[i])];

				if (attack && attack != reference[i])
					break;

				attack = reference[i];
			}

		} while (i < size);

		printf("sq:%d,imagic:0x%llX,  size: %d\n", s, BishopLegImagic[s], size);

		
	}
	
}

void init_slider_magics(Bitboard table[],
	Bitboard* attacks[],
	Bitboard magics[],
	Bitboard masks[],
	unsigned shifts[],
	Square deltas[],
	IndexFn index,
	AttackFun attackfun,
	Bitboard magicsdata[],
	unsigned datashifts[]) {

	Bitboard occupancy[1 << 15], reference[1 << 15], edges, b;
	int i, size;

	// attacks[s] is a pointer to the beginning of the attacks table for square 's'
	attacks[SQ_A0] = table;

	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		// Board edges are not considered in the relevant occupancies
		edges = ((Rank0BB | Rank9BB) & ~rank_bb(s)) | ((FileABB | FileIBB) & ~file_bb(s));

		masks[s] = sliding_attack(deltas, s, Bitboard()) & ~edges;
		shifts[s] = 64 - pop_count(masks[s]);

		size = 0;
		b = Bitboard();
		do
		{
			do
			{
				occupancy[size] = b;
				reference[size] = attackfun(deltas, s, b);

				size++;

				b.bb[0] = (b.bb[0] - masks[s].bb[0])&masks[s].bb[0];

			} while (b.bb[0]);
			b.bb[1] = (b.bb[1] - masks[s].bb[1])&masks[s].bb[1];

		} while (b.bb[1]);

		if (s < SQ_I9)
			attacks[s + 1] = attacks[s] + size;

		if (shifts[s] != datashifts[s]) {
			printf("sq%d shift error\n",s);
		}

		do {

			magics[s] = magicsdata[s];

			std::memset(attacks[s], 0, size * sizeof(Bitboard));

			for (i = 0; i < size; ++i)
			{
				Bitboard& attack = attacks[s][index(s, occupancy[i])];

				if (attack && attack != reference[i])
					break;

				attack = reference[i];
			}

		} while (i < size);
		printf("sq:%d,bb0:0x%llX,bb1:0x%llX  size: %d\n", s, magics[s].bb[0], magics[s].bb[1], size);

	}
}
void init_knight_maigcs(Bitboard attacktable[SQUARE_NB][16],
	Bitboard legoreye[SQUARE_NB],
	uint64_t imagic[SQUARE_NB],
	IndexFn index,
	KAttackFun attackfun,
	uint64_t magicsdata[]	
	)
{ 
	Bitboard occupancy[1 << 4], reference[1 << 4], edges, b;
	int i, size, booster;

	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		size = 0;
		b = Bitboard();		
		do
		{	do
			{
				occupancy[size] = b;
				reference[size] = attackfun(s, b);
				size++;
				b.bb[0] = (b.bb[0] - legoreye[s].bb[0])&legoreye[s].bb[0];
			} while (b.bb[0]);
			b.bb[1] = (b.bb[1] - legoreye[s].bb[1])&legoreye[s].bb[1];

		} while (b.bb[1]);		

		do {
			imagic[s] = magicsdata[s];
			std::memset(attacktable[s], 0, size * sizeof(Bitboard));

			for (i = 0; i < size; ++i)
			{
				Bitboard& attack = attacktable[s][index(s, occupancy[i])];
				if (attack && attack != reference[i])
					break;
				attack = reference[i];
			}

		} while (i < size);
		printf("sq:%d,imagic:0x%llX,  size: %d\n", s, imagic[s], size);
	}
}
void init_bishop_magics(uint64_t magicsdata[]) {

	Bitboard occupancy[1 << 4], reference[1 << 4], b;
	int i, size, booster;

	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		//s不在相应该在的位置上
		if (!bishop_in_city(s)) {
			continue;
		}
		size = 0;
		b = Bitboard();		
		do
		{
			do
			{
				occupancy[size] = b;
				reference[size] = bishop_attack_to(s, b);
				size++;
				b.bb[0] = (b.bb[0] - BishopLeg[s].bb[0])&BishopLeg[s].bb[0];
			} while (b.bb[0]);
			b.bb[1] = (b.bb[1] - BishopLeg[s].bb[1])&BishopLeg[s].bb[1];

		} while (b.bb[1]);	

		do {
			BishopLegImagic[s] = magicsdata[s];
			std::memset(BishopImagicAttack[s], 0, size * sizeof(Bitboard));
			for (i = 0; i < size; ++i)
			{
				Bitboard& attack = BishopImagicAttack[s][bishop_imagic_index(s, occupancy[i])];
 				if (attack && attack != reference[i])
					break;
				attack = reference[i];
			}
		} while (i < size);
		printf("sq:%d,imagic:0x%llX,  size: %d\n", s, BishopLegImagic[s], size);
	}
}
