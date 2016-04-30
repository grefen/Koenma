#ifndef BITBOARD_H
#define BITBOARD_H
#include <cstdint>
#include <cstdio>

//位棋盘，棋盘映射如下：
//bit map to square as follow:
/*
81, 82, 83, 84, 85, 86, 87, 88, 89,
72, 73, 74, 75, 76, 77, 78, 79, 80,
63, 64, 65, 66, 67, 68, 69, 70, 71,
54, 55, 56, 57, 58, 59, 60, 61, 62,
45, 46, 47, 48, 49, 50, 51, 52, 53,
36, 37, 38, 39, 40, 41, 42, 43, 44,
27, 28, 29, 30, 31, 32, 33, 34, 35,
18, 19, 20, 21, 22, 23, 24, 25, 26,
9,  10, 11, 12, 13, 14, 15, 16, 17,
0,  1,  2,  3,  4,  5,  6,  7,  8, 
*/

class Bitboard
{
	// 0~44bit and 0~44 bit
public:
	static const uint64_t BIT_MASK = 0x1FFFFFFFFFFF;
public:
	uint64_t bb[2];

	Bitboard(){ bb[0] = bb[1] = 0;}
	Bitboard(uint64_t low, uint64_t hig){bb[0] = low&BIT_MASK; bb[1] = hig&BIT_MASK;}
	
	operator bool()const{
		return bb[0]||bb[1];
	}

	int operator == (const Bitboard& board)const{
		return bb[0] == board.bb[0] &&
			   bb[1] == board.bb[1] ;
	}

	int operator != (const Bitboard& board)const{
		return bb[0] != board.bb[0] ||
			   bb[1] != board.bb[1] ;
	}

	Bitboard operator ~()const{
		return Bitboard(~bb[0],~bb[1]);//最高取反产生垃圾数据
	}

	Bitboard operator &(const Bitboard& board)const{
		return Bitboard(bb[0]&board.bb[0],bb[1]&board.bb[1]);
	}

	Bitboard operator |(const Bitboard& board)const{
        return Bitboard(bb[0]|board.bb[0],bb[1]|board.bb[1]);
	}

	Bitboard operator ^(const Bitboard &board) const 
	{
		return Bitboard(bb[0]^board.bb[0],bb[1]^board.bb[1]);
	}

	Bitboard &operator &=(const Bitboard &board) 
	{
		bb[0] &= board.bb[0];
		bb[1] &= board.bb[1];

		return *this;
	}

	Bitboard &operator |=(const Bitboard &board) 
	{
		bb[0] |= board.bb[0];
		bb[1] |= board.bb[1];
		return *this;
	}

	Bitboard &operator ^=(const Bitboard &board) 
	{
		bb[0] ^= board.bb[0];
		bb[1] ^= board.bb[1];

		return *this;
	}

	Bitboard operator <<(int bit) 
	{ 
		//最高取反产生垃圾数据
		if (bit < 0)
			return *this >> -bit;
		else if (bit <= 45)
            return Bitboard(bb[0] << bit, bb[1] << bit | bb[0] >> (45 - bit));
		else if(bit <= 90)
			return Bitboard(0, bb[0]<<(bit-45));
		else
			return Bitboard(0, 0);
	}

	Bitboard operator >>(int bit) 
	{
		if (bit < 0)
			return *this << -bit;
		else if(bit <= 45)
			return Bitboard(bb[0] >> bit | bb[1] << (45 - bit), bb[1] >> bit );
		else if(bit <= 90)
            return Bitboard(bb[1] >> (bit - 45), 0);
		else
			return Bitboard(0, 0);
	}

	Bitboard &operator <<=(int bit)
	{
		if (bit < 0){
			*this >>= -bit;
		} 
		else if (bit <= 45)
		{
			bb[1] <<= bit;
			bb[1] |= bb[0]>>(45-bit);
			bb[0] <<= bit;

			bb[0] &= BIT_MASK;//垃圾数据清理
			bb[1] &= BIT_MASK;//垃圾数据清理
		}
		else if (bit <= 90)
		{
			bb[1] = bb[0]<<(bit-45);
			bb[0] = 0;

			bb[1] &= BIT_MASK;//垃圾数据清理
		}
		else
		{
			bb[0] = 0;
			bb[1] = 0;
		}

		return *this;
	}

	Bitboard &operator >>=(int bit)
	{
		if (bit < 0){
			*this <<= -bit;
		} 
		else if (bit <= 45)
		{
            bb[0] >>= bit;
            bb[0] |= bb[1] << (45 - bit);
			bb[1] >>= bit;

			bb[0] &= BIT_MASK;//垃圾数据清理
		}
		else if (bit <= 90)
		{
			bb[0] = bb[1] >> (bit - 45);
			bb[1] = 0;
		}
		else
		{ 
            bb[0] = 0;
            bb[1] = 0;
		}

		return *this;
	}

	void pop_lsb()
	{
		if (bb[0])
		{
			bb[0] &= bb[0] - 1;
		}
		else if (bb[1])
		{
			bb[1] &= bb[1] - 1;
		}

	}

	bool more_than_one()const
	{
		uint32_t c = 0;
		if (bb[0])
		{
			if (bb[0] & (bb[0] - 1)) return true; // > 1
			++c;
		}

		if (bb[1])
		{
			if(c)  return true;	// > 1
			if (bb[1] & (bb[1] - 1)) return true; // > 1
			++c;
		}

		return c > 1;
	}

	bool equal_to_two()const
	{
		uint32_t c = 0;
		uint64_t t = bb[0];

		while (t) {
			t &= t - 1;
			++c;
			if (c > 2) return false;
		}

		t = bb[1];
		while (t) {
			t &= t - 1;
			++c;
			if (c > 2) return false;
		}
		
		return c == 2;
	}

	//for debug
	void print(FILE* fp) const
	{
		fprintf(fp,"\n");
		
		int shift[90] = {
			81, 82, 83, 84, 85, 86, 87, 88, 89,
			72, 73, 74, 75, 76, 77, 78, 79, 80,
			63, 64, 65, 66, 67, 68, 69, 70, 71,
			54, 55, 56, 57, 58, 59, 60, 61, 62,
			45, 46, 47, 48, 49, 50, 51, 52, 53,
			36, 37, 38, 39, 40, 41, 42, 43, 44,
			27, 28, 29, 30, 31, 32, 33, 34, 35,
			18, 19, 20, 21, 22, 23, 24, 25, 26,
			9,  10, 11, 12, 13, 14, 15, 16, 17,
			0,  1,  2,  3,  4,  5,  6,  7,  8,
		};
		Bitboard one(0x1,0);
		for (int i = 0; i < 90; ++i)
		{
            Bitboard t = one<<shift[i];
            fprintf(fp,"%d",(t&(*this)) ? 1 : 0);

			if ((i+1)%9 == 0)
			{
				fprintf(fp,"\n");
			}

			if(i+1 == 45)
			{
               fprintf(fp,"---------\n");
			}
		}
	}
};

#endif