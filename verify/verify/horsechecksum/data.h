#ifndef data_h
#define data_h
#include "bitboard.h"

#define BIT(s)		  (BitMask[s])
#define SQ(x,y)       (RankBase[x] + (y))
#define HORSELEGS(sq) (HorseLegs[sq])
#define INBOARD(x, y) ((x) >= 0 && (x) <= 8 && (y) >= 0 && (y) <= 9)   


extern Bitboard BitMask[90];

extern int      RankBase[9];

extern Bitboard KnightLegs[90];
extern Bitboard KnightLegsEx[90];

extern const int SqToFileEx[90];
extern const int SqToRankEx[90];
extern const int SqToFile[90];
extern const int SqToRank[90];

extern void init();


#endif