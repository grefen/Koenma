#include<stdio.h>
#include<assert.h>
#include<Windows.h>
#include<stdint.h>
#include <intrin.h>

typedef unsigned __int16 uint16_t;
typedef          __int16 int16_t;
int make_score(int m, int e) {
	int v = (m << 16) + e;

	return v;
}
inline int mg_value(int s) {

	union { uint16_t u; int16_t s; } mg = { uint16_t(unsigned(s + 0x8000) >> 16) };
	return int(mg.s);
}

inline int eg_value(int s) {

	union { uint16_t u; int16_t s; } eg = { uint16_t(unsigned(s)) };
	return int(eg.s);
}
inline int relative_square(int c, int s) {
	return int(s ^ (c * 56)); //vertical flip
}
inline int relative_rank(int c, int r) {
	return int(r ^ (c * 7));
}

inline int rank_of(int s) {
	return int(s >> 3);
}
//inline int relative_rank(int c, int s) {
//	return relative_rank(c, rank_of(s));
//}

inline bool opposite_colors(int s1, int s2) {
	int s = int(s1) ^ int(s2);
	return ((s >> 3) ^ s) & 1;
}
inline int flip(int s) {
	return int(s ^ 56); // Vertical flip SQ_A1 -> SQ_A8
}
inline int popcount(uint64_t b) {
	b -= (b >> 1) & 0x5555555555555555ULL;
	b = ((b >> 2) & 0x3333333333333333ULL) + (b & 0x3333333333333333ULL);
	return (b * 0x1111111111111111ULL) >> 60;
}
struct Bitboard {
	uint64_t bb[2];
};
inline int lsb(uint64_t b) {
	unsigned long idx;
	_BitScanForward64(&idx, b);
	return (int)idx;
}
inline int lsb(Bitboard b) {
	unsigned long idx;

	if (b.bb[0]) _BitScanForward64(&idx, b.bb[0]);
	else if (b.bb[1]) {
		_BitScanForward64(&idx, b.bb[1]);
		idx += 45;
	}

	return (int)idx;
}
inline int msb(uint64_t b) {
	unsigned long idx;
	_BitScanReverse64(&idx, b);
	return (int)idx;
}
inline int msb(Bitboard b) {
	unsigned long idx;

	if (b.bb[1]) {
		_BitScanReverse64(&idx, b.bb[1]);
		idx += 45;
	}
	else if (b.bb[0])	 _BitScanReverse64(&idx, b.bb[0]);

	return (int)idx;
}
int main() {

	int score;

	//for (int i = -32766; i < 32765; ++i) {

	//	int v = make_score(i, i);
	//	printf("0x%x ",v );

	//	assert(mg_value(v) == i);
	//	assert(eg_value(v) == i);
	//}

	//for (int i = 0; i < 64; ++i) printf("%d ", relative_square(1,i));
	//for (int i = 0; i < 8; ++i) printf("%d ", relative_rank(1, i));
	//for (int i = 0; i < 64; ++i) printf("%d ", relative_rank(1, rank_of(i)));
	//for (int i = 0; i < 64; ++i) printf("%d ", opposite_colors(i,i+1));
	//for (int i = 0; i < 64; ++i) printf("%d ", flip(i));

	//for (int i = 0; i < 64; ++i) {
	//	//uint64_t t = 0x1LLU<<i;
	//	//printf("%d ", lsb(t));
	//	Bitboard t{0,0};
	//	t.bb[0] = 0x3LLU << i;
	//	//printf("%d ", lsb(t));
	//	//t.bb[1] = 0x1LLU << i;
	//	printf("%d ", msb(t));
	//}
	Bitboard bb = { 0, 34978264427008 };

	uint64_t b = 34978264427008;
	for (int i = 45; i < 90; ++i) {
		printf("sq %d ",i);
		if (b&((0x1llU) << (i - 45)))
			printf("1");
		else
			printf("0");
		printf("\n");
	}
	
	int m = 17985;
	//printf("from %d to %d ", m>>8, m&0xff);

	system("pause");
	return 0;
}