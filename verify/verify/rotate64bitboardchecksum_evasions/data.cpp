#include <algorithm>
#include <stdlib.h>
#include "data.h"
#include "rkiss.h"
#include "bitcount.h"

CACHE_LINE_ALIGNMENT

Bitboard SquareBB[SQUARE_NB];
Bitboard SquareBBL90[SQUARE_NB];
Bitboard FileBB[FILE_NB];
Bitboard RankBB[RANK_NB];

Bitboard CityBB[COLOR_NB];
Bitboard AdvisorCityBB[COLOR_NB];
Bitboard BishopCityBB[COLOR_NB];

Bitboard RookAttackMask[SQUARE_NB];
Bitboard RookAttackToR0[SQUARE_NB][128];
Bitboard RookAttackToRL90[SQUARE_NB][256];

Bitboard KnightAttackMask[SQUARE_NB];
Bitboard KnightAttackTo[SQUARE_NB][256];
Bitboard KnightLeg[SQUARE_NB];
Bitboard KnightLegRL90[SQUARE_NB];
Bitboard KnightAttackFrom[SQUARE_NB][256];
Bitboard KnightEye[SQUARE_NB];
Bitboard KnightEyeRL90[SQUARE_NB];

Bitboard CannonAttackToR0[SQUARE_NB][128];
Bitboard CannonAttackToRL90[SQUARE_NB][256];
Bitboard CannonSuperR0[SQUARE_NB][128];
Bitboard CannonSuperRL90[SQUARE_NB][256];

Bitboard BishopAttackTo[SQUARE_NB][256];
Bitboard BishopLeg[SQUARE_NB];
Bitboard BishopLegRL90[SQUARE_NB];

Bitboard AdvisorAttackTo[SQUARE_NB];

Bitboard KingAttackTo[SQUARE_NB];

Bitboard PawnAttackTo[COLOR_NB][SQUARE_NB];
Bitboard PawnAttackFrom[COLOR_NB][SQUARE_NB];
Bitboard PawnMask[COLOR_NB];
Bitboard PassedRiverBB[COLOR_NB];

int SquareDistance[SQUARE_NB][SQUARE_NB];

int    MS1BTable[256];
Square BSFTable[128];

Bitboard RookTable[90*(1<<15)];//45M mem, very big
Bitboard CannonTable[90*(1<<15)];

Bitboard  RMasks[SQUARE_NB];
Bitboard* RAttacks[SQUARE_NB];
Bitboard  RMagics[SQUARE_NB];
unsigned  RShifts[SQUARE_NB];

Bitboard  CannonMasks[SQUARE_NB];
Bitboard* CannonAttacks[SQUARE_NB];
Bitboard  CannonMagics[SQUARE_NB];
unsigned  CannonShifts[SQUARE_NB];

Bitboard BetweenBB[SQUARE_NB][SQUARE_NB];

const uint64_t DeBruijn_64 = 0x3F79D71B4CB0A89ULL;
const uint32_t DeBruijn_32 = 0x783A9B23;

typedef unsigned (Fn)(Square, Bitboard);

void init_magics(Bitboard table[], Bitboard* attacks[], Bitboard magics[],
				 Bitboard masks[], unsigned shifts[], Square deltas[], Fn index);

//my own for test
//uint32_t random_uint32() {
//	uint32_t u1, u2;
//	u1 = (uint32_t)(rand()) & 0xFFFF; u2 = (uint32_t)(rand()) & 0xFFFF;	
//	return u1 | (u2 << 16);
//}
//
//Bitboard random_bitboard(){
//	return Bitboard(random_uint32(),random_uint32(),random_uint32());
//}

uint32_t pop_count(const Bitboard& b)
{
	return popcount<CNT_64>(b.bb[0]) + popcount<CNT_64>(b.bb[1]);
}

Bitboard sliding_attack(Square deltas[], Square sq, Bitboard occupied) {

	Bitboard attack(0,0);

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

Bitboard cannon_sliding_control(Square deltas[], Square sq, Bitboard occupied) {

	Bitboard attack(0,0);

	//square_distance作用是防止边界发生回环，如最右边的sq加1会回环到最左上位置
	for (int i = 0; i < 4; ++i){
		int count = 0;
		for (Square s = sq + deltas[i];	is_ok(s) && square_distance(s, s - deltas[i]) == 1;
			s += deltas[i])
		{
			if ( count == 1)
			{
				attack |= s;
			}
			else if ( count >= 2 )
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

//
uint32_t bsf_index90(Bitboard b) {

	//32bit
	//if(b.bb[0] > 0)
	//{
	//	return (((b.bb[0]^(b.bb[0]-1))* DeBruijn_32)>>27);
	//}
	//else if(b.bb[1] > 0)
	//{
	//	return (((b.bb[1]^(b.bb[1]-1))* DeBruijn_32)>>27) + 32;
	//}
	//else if(b.bb[2] > 0)
	//{
	//	return (((b.bb[2]^(b.bb[2]-1))* DeBruijn_32)>>27) + 64;
	//}

	//64bit
	if(b.bb[0] > 0)
	{
		return (((b.bb[0]^(b.bb[0]-1))* DeBruijn_64)>>58);
	}
	else if(b.bb[1] > 0)
	{
		return (((b.bb[1]^(b.bb[1]-1))* DeBruijn_64)>>58) + 64;
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

	//uint32_t b32;
	//int result = 0;

	//if(b.bb[2] > 0)
	//{
	//	result = 64;

	//	b32 = uint32_t(b.bb[2]);

	//	if (b32 > 0xFFFF)
	//	{
	//		b32 >>= 16;
	//		result += 16;
	//	}

	//	if (b32 > 0xFF)
	//	{
	//		b32 >>= 8;
	//		result += 8;
	//	}
	//}
	//else if(b.bb[1] > 0)
	//{
	//	result = 32;

	//	b32 = uint32_t(b.bb[1]);

	//	if (b32 > 0xFFFF)
	//	{
	//		b32 >>= 16;
	//		result += 16;
	//	}

	//	if (b32 > 0xFF)
	//	{
	//		b32 >>= 8;
	//		result += 8;
	//	}
	//}
	//else 
	//{
	//	b32 = uint32_t(b.bb[0]);

	//	if (b32 > 0xFFFF)
	//	{
	//		b32 >>= 16;
	//		result += 16;
	//	}

	//	if (b32 > 0xFF)
	//	{
	//		b32 >>= 8;
	//		result += 8;
	//	}
	//}  

	//return (Square)(result + MS1BTable[b32]);
}
Square lsb(Bitboard b)
{
	return BSFTable[bsf_index90(b)];
}
Square pop_lsb(Bitboard* b)
{
	Bitboard bb = *b;
	//*b = bb & (bb.operator-(1));
	b->pop_lsb();
	return BSFTable[bsf_index90(bb)];
}
//inline uint32_t popcount32(uint32_t b)
//{
//	uint32_t  v = uint32_t(b);
//	v -=  (v >> 1) & 0x55555555; // 0-2 in 2 bits
//	v  = ((v >> 2) & 0x33333333) + (v & 0x33333333); // 0-4 in 4 bits
//	v  = ((v >> 4) + v) & 0x0F0F0F0F;
//	return (v * 0x01010101) >> 24;
//}
inline int popcount64(uint64_t b) {
	b -=  (b >> 1) & 0x5555555555555555ULL;
	b  = ((b >> 2) & 0x3333333333333333ULL) + (b & 0x3333333333333333ULL);
	b  = ((b >> 4) + b) & 0x0F0F0F0F0F0F0F0FULL;
	return (b * 0x0101010101010101ULL) >> 56;
}
inline uint32_t popcount(const Bitboard& b){
    //return popcount32(b.bb[0]) + popcount32(b.bb[1]) + popcount32(b.bb[2]);
	return popcount64(b.bb[0]) + popcount64(b.bb[1]);
}

void init_data(){

	for (int k = 0, i = 0; i < 8; ++i)
		while (k < (2 << i))
			MS1BTable[k++] = i;

	for (int i = 0; i < SQUARE_NB; ++i)
	{
		BSFTable[bsf_index90(Bitboard(1,0) << i)] = Square(i);	 
	}


	for (int i = 0; i < SQUARE_NB; ++i)
	{
		SquareBB[i] = (Bitboard(1, 0)<<(i));
	}

	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		SquareBBL90[s] = SquareBB[square_rotate_l90(s)];
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

	Square RDeltas[] = { DELTA_N,  DELTA_E,  DELTA_S,  DELTA_W  };
	for(Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		RookAttackMask[s]  = sliding_attack(RDeltas, s, Bitboard()) ;
	}

	for(Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		// 边子是无关紧要的，即不管有没有子，都认为可以吃到，这样可以节省存储空间
		//原本2^9变为2^7=128的大小，同理2^10变为2^8=256的大小

		// rook rank		
		for(int i = 0; i < 128; ++i)
		{         
			int f;
			int p;
			for(f = file_of(s)-1, p = s - 1; f >= 0; p--, f--) {
				RookAttackToR0[s][i] |= SquareBB[p];
				if((i<<1)&(1<<f))	 break;
			} for(f = file_of(s) + 1, p = s + 1; f < 9; p++, f++) {
				RookAttackToR0[s][i] |= SquareBB[p];
				if((i<<1)&(1<<f))	 break;
			}
		}
		// rook file
		for(int i = 0; i < 256; ++i)
		{
			int r;
			int p;
			for(r = rank_of(s) - 1, p = s - 9; r >= 0; p -= 9, r--) {
				RookAttackToRL90[s][i] |= SquareBB[p];
				if((i<<1)&(1<<(9-r)))break;
			} for(r = rank_of(s) + 1, p = s + 9; r < 10; p += 9, r++) {
				RookAttackToRL90[s][i] |= SquareBB[p];
				if((i<<1)&(1<<(9-r)))break;
			}
		}
	}


	for(Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		for(int i = 0; i < 128; ++i)
		{         
			int f;
			int p;
			for(f = file_of(s)-1, p = s - 1; f >= 0; p--, f--) {			 
				if((i<<1)&(1<<f))	 break;// battery
			} 
			for(p--, f--; f >= 0; p--, f--) {
				CannonAttackToR0[s][i] |= SquareBB[p];
				if((i<<1)&(1<<f))	 break;
			}
			for(p--, f--; f >= 0; p--, f--) {	
				CannonSuperR0[s][i] |= SquareBB[p];
				if((i<<1)&(1<<f))
				{
					break;
				}
			}


			for(f = file_of(s) + 1, p = s + 1; f < 9; p++, f++) {			 
				if((i<<1)&(1<<f))	 break;// battery
			}
			for(p++, f++; f < 9; p++, f++) {
				CannonAttackToR0[s][i] |= SquareBB[p];
				if((i<<1)&(1<<f))	 break;
			}
			for(p++, f++; f < 9; p++, f++) {			  
				CannonSuperR0[s][i] |= SquareBB[p];
				if((i<<1)&(1<<f))
				{

					break;
				}
			}
		}

		// cannon file
		for(int i = 0; i < 256; ++i)
		{
			int r;
			int p;
			for(r = rank_of(s) - 1, p = s - 9; r >= 0; p -= 9, r--) {
				if((i<<1)&(1<<(9-r)))break;// battery
			}
			for(p -= 9, r--; r >= 0; p -= 9, r--) {
				CannonAttackToRL90[s][i] |= SquareBB[p];
				if((i<<1)&(1<<(9-r)))break;
			}
			for(p -= 9, r--; r >= 0; p -= 9, r--) {
				CannonSuperRL90[s][i] |= SquareBB[p];
				if((i<<1)&(1<<(9-r)))
				{
					break;
				}
			}

			for(r = rank_of(s) + 1, p = s + 9; r < 10; p += 9, r++) {
				if((i<<1)&(1<<(9-r)))break;// battery
			}
			for(p += 9, r++; r < 10; p += 9, r++) {
				CannonAttackToRL90[s][i] |= SquareBB[p];
				if((i<<1)&(1<<(9-r)))break;
			}
			for(p += 9, r++; r < 10; p += 9, r++) {	
				CannonSuperRL90[s][i] |= SquareBB[p];
				if((i<<1)&(1<<(9-r)))
				{

					break;
				}
			}
		}
	}


	//knight leg
	//top east souse west
	Square KnightLegDeltas[4] = {DELTA_N, DELTA_E,DELTA_S,DELTA_W};
	Square KnightStep[4][2]   = {{DELTA_NNW,DELTA_NNE},{DELTA_EEN, DELTA_EES},{DELTA_SSE, DELTA_SSW},{DELTA_WWS,DELTA_WWN}};
	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		KnightLegRL90[s] = KnightLeg[s] = Bitboard();
		for (int i = 0; i< 4; ++i)
		{
			int to = (int)s + (int)KnightLegDeltas[i];//枚举类型可能不存在，所以用int类型
			if (is_ok(to) && square_distance(Square(s), Square(to)) == 1)//在棋盘上，并且没有发生回旋
			{
				KnightLeg[s] |= SquareBB[to];

				KnightLegRL90[s] |= SquareBBL90[to];//没有旋转的sq位置是正确的，旋转棋盘的位置必定是正确的
			}
		}

		for (int i = 0; i < 256; ++i)
		{
			KnightAttackTo[s][i] = Bitboard();

			//使用旋转棋盘
			//Bitboard Knightleg = duplicate(i, KnightLeg[s]);
			Bitboard legRL90 = duplicate(i, KnightLegRL90[s]);

			for (int j = 0; j < 4; ++j)//四个方向
			{
				for (int k = 0; k < 2; ++k)//每个方向两个马脚
				{
					int to = int(s) + KnightStep[j][k];//马脚
					if (is_ok(to) && square_distance(Square(s), Square(to)) < 3)//在棋盘上，并且没有发生回旋
					{
						int legsq = int(s) + KnightLegDeltas[j];//马腿
						if (is_ok(legsq) && square_distance(Square(s), Square(legsq)) == 1)
						{
							if ( !(legRL90&SquareBBL90[legsq]) )//不存在马腿
							{
								KnightAttackTo[s][i] |= SquareBB[to];//i是用旋转棋盘计算的checksum
							}


						}

						KnightAttackMask[s] |= SquareBB[to];//无需顾虑马腿问题
					}
				}
			}
		}
	}

	//knight eye
	Square KnightEyeDeltas[4] = {DELTA_NW, DELTA_NE, DELTA_SE, DELTA_SW};
	Square KnightStepFrom[4][2]  = {{DELTA_WWN,DELTA_NNW},{DELTA_NNE,DELTA_EEN},{DELTA_EES,DELTA_SSE},{DELTA_SSW, DELTA_WWS}};
	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		KnightEyeRL90[s] = KnightEye[s] = Bitboard();
		for (int i = 0; i < 4; ++i)
		{
			int to = (int)s + (int)KnightEyeDeltas[i];
			if (is_ok(to) && square_distance(Square(s), Square(to)) == 1)
			{
				KnightEye[s] |= SquareBB[to];

				KnightEyeRL90[s] |= SquareBBL90[to];
			}
		}

		for (int i = 0; i < 256; ++i)
		{
			KnightAttackFrom[s][i] = Bitboard();

			//Bitboard KnightEye = duplicate(i, KnightEye[s]);
			Bitboard EyeRL90 = duplicate(i, KnightEyeRL90[s]);
			for (int j = 0; j < 4; ++j)
			{
				for (int k = 0; k < 2; ++k)
				{
					int to = int(s) + (int)KnightStepFrom[j][k];
					if (is_ok(to) && square_distance(Square(s), Square(to)) < 3)
					{
						int sqeye = int(s) + int(KnightEyeDeltas[j]);
						if (is_ok(sqeye) && square_distance(Square(s), Square(sqeye)) == 1)
						{
							if ( !(EyeRL90&SquareBBL90[sqeye]) )
							{
								KnightAttackFrom[s][i] |= SquareBB[to];//i是用旋转棋盘计算的checksum
							}
						}
					}
				}
			}
		}
	}


	//bishop
	Square BishopEyeDeltas[4] = {DELTA_NW, DELTA_NE, DELTA_SE, DELTA_SW};
	Square BishopStepDeltas[4]={DELTA_NNWW, DELTA_NNEE, DELTA_SSEE, DELTA_SSWW};
	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		BishopLegRL90[s] = BishopLeg[s] = Bitboard();

		for (int i = 0; i < 4; ++i)
		{
			int to = (int)s + (int)BishopEyeDeltas[i];
			if (is_ok(to) && square_distance(Square(s), Square(to)) == 1 && square_same_color(Square(s), Square(to)))
			{
				BishopLeg[s] |= SquareBB[to];
				BishopLegRL90[s] |= SquareBBL90[to]; 
			}
		}

		for (int i = 0; i < 256; ++i)
		{
			BishopAttackTo[s][i] = Bitboard();

			Bitboard BishopEyeL90 = duplicate(i, BishopLegRL90[s]);

			for (int j = 0; j < 4; ++j)
			{
				int to = (int)s + BishopStepDeltas[j];
				if (is_ok(to) && square_distance(Square(s), Square(to)) < 3 && square_same_color(Square(s), Square(to)) && bishop_in_city(Square(to)) && bishop_in_city(Square(s)))
				{
					int eyesq = (int)s + BishopEyeDeltas[j];
					if (is_ok(eyesq) && square_distance(Square(s), Square(eyesq)) == 1 && square_same_color(Square(s), Square(eyesq)))
					{
						if ( !(BishopEyeL90&SquareBBL90[eyesq]) )
						{
							BishopAttackTo[s][i] |= SquareBB[to];
						}
					} 

				}
			}
		}
	}

	//advisor
	Square AdvisorDetas[4] = {DELTA_NW, DELTA_NE, DELTA_SE, DELTA_SW};
	for (Square s = SQ_A0; s <= SQ_I9; ++s)
	{
		AdvisorAttackTo[s] = Bitboard();
		for (int i = 0; i < 4; ++i)
		{
			int to = (int)s + AdvisorDetas[i];
			if (is_ok(to) && square_distance(Square(s), Square(to)) == 1 && advisor_in_city(Square(to))&& advisor_in_city(Square(s)))
			{
				AdvisorAttackTo[s] |= SquareBB[to];
			}
		}
	}

	//pawn
	Square PawnDetas[COLOR_NB][3] = {{DELTA_N,DELTA_W, DELTA_E},{DELTA_S,DELTA_E,DELTA_W}};
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
						if (PawnMask[c]&SquareBB[to])//mask是pawn的正确位置掩码，落在mask里面得必然是正确的
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
						if (PawnMask[opp]&SquareBB[to])//如果to在对方的mask里面，必定是对方合理的走法
						{						 
							PawnAttackFrom[opp][s] |=  SquareBB[to];//s位置遭受对方攻击
						}
					}

				}
			}

		}
	}


	//king
	Square KingDetas[4] = {DELTA_N, DELTA_E, DELTA_S,DELTA_W};
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


	//init imagics
	//Square RDeltas[] = { DELTA_N,  DELTA_E,  DELTA_S,  DELTA_W  };
   //init_magics(RookTable, RAttacks, RMagics, RMasks, RShifts, RDeltas, magic_index<ROOK>);
   // init_magics(CannonTable, CannonAttacks, CannonMagics, CannonMasks, CannonShifts, RDeltas, magic_index<ROOK>);
}

// init_magics() computes all rook and bishop attacks at startup. Magic
// bitboards are used to look up attacks of sliding pieces. As a reference see
// chessprogramming.wikispaces.com/Magic+Bitboards. In particular, here we
// use the so called "fancy" approach.

void init_magics(Bitboard table[], Bitboard* attacks[], Bitboard magics[], Bitboard masks[], unsigned shifts[], Square deltas[], Fn index) {

	//int MagicBoosters[][10] = { {  969, 1976, 2850,  542, 2069, 2852, 1708,  164, 164, 164 },
	//{ 3101,  552, 3555,  926,  834,   26, 2131, 1117, 1117, 1117 } };

	int MagicBoosters[][10] = { {  969, 1976, 2850,  969, 2069, 2852, 1708,  164, 164, 164 },
	{ 3101,  552, 3555,  926,  834,   26, 2131, 1117, 1117, 1117 } };

	RKISS rk;
	Bitboard occupancy[1<<15], reference[1<<15], edges, b;
	int i, size, booster;

	int sum = 0;

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
		masks[s]  = sliding_attack(deltas, s, Bitboard()) & ~edges;
		shifts[s] = 64 - popcount(masks[s]);//(Is64Bit ? 64 : 32) - popcount<Max15>(masks[s]);

		// Use Carry-Rippler trick to enumerate all subsets of masks[s] and
		// store the corresponding sliding attack bitboard in reference[].
		//b = size = 0;
		size = 0;
		//do {
		//	occupancy[size] = b;
		//	reference[size] = sliding_attack(deltas, s, b);

		//	//if (HasPext)
		//	//	attacks[s][_pext_u64(b, masks[s])] = reference[size];

		//	size++;
		//	b = (b - masks[s]) & masks[s];
		//} while (b);

		//遍历所有bit
		//do 
		//{
			do 
			{
				do 
				{
					occupancy[size] = b;
					reference[size] = sliding_attack(deltas, s, b);

					size++;


					b.bb[0] = (b.bb[0] - masks[s].bb[0])&masks[s].bb[0];

				} while (b.bb[0]);
				b.bb[1] = (b.bb[1] - masks[s].bb[1])&masks[s].bb[1];

			} while (b.bb[1]);
			//b.bb[2] = (b.bb[2] - masks[s].bb[2])&masks[s].bb[2];
		//} while (b.bb[2]);


		// Set the offset for the table of the next square. We have individual
		// table sizes for each square with "Fancy Magic Bitboards".
		if (s < SQ_I9)
			attacks[s + 1] = attacks[s] + size;

		//if (HasPext)
		//	continue;

		booster = MagicBoosters[1][rank_of(s)];

		// Find a magic for square 's' picking up an (almost) random number
		// until we find the one that passes the verification test.

		int seed = 3;
		int trycount = 0;
		int maxtime = 0;
		do {
			//do magics[s] = rk.magic_rand<Bitboard>(booster);
			//while (popcount<Max15>((magics[s] * masks[s]) >> 56) < 6);
			do 
			{
				magics[s] = rk.magic_bitboard(booster);               
				
			} while (popcount64(( (magics[s].bb[0] * masks[s].bb[0])^(magics[s].bb[1] * masks[s].bb[1]) ) >> 54) < 5);

			trycount++;
			

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

				assert(reference[i]);

				attack = reference[i];
			}

			if(i > maxtime) maxtime = i;
		} while (i < size);

		
		printf("sq:%d,bb0:0x%llX,bb1:0x%llX  size: %d\n",s, magics[s].bb[0], magics[s].bb[1], size);

		sum+= size;
	}

	printf("sum: %d\n", sum);

	printf("rook need %d byte == %dm memory\n", sum*16, (sum*16)/1024/1024);
}