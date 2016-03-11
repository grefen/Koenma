#ifndef CHECKSUM_H
#define CHECKSUM_H
#include "bitboard.h"

//使用折叠位棋盘，要求是：square必须按照列来排列，而旋转的位棋盘正是按照列来排列的，所以非常适合折叠位棋盘
inline uint8_t checksum(const Bitboard& board) 
{
	//uint32_t  Temp32 = board.bb[0] | board.bb[1] | board.bb[2];
	//uint16_t  Temp16 = ((uint16_t *) &Temp32)[0] | ((uint16_t *) &Temp32)[1];
	//return (uint8_t) (((uint8_t *) &Temp16)[0] | ((uint8_t *) &Temp16)[1]);

	uint64_t Temp64 = board.bb[0] |  board.bb[1];
    uint32_t Temp32 = ((uint32_t *) &Temp64)[0] | ((uint32_t *) &Temp64)[1];
	uint16_t Temp16 = ((uint16_t *) &Temp32)[0] | ((uint16_t *) &Temp32)[1];
	return (uint8_t) (((uint8_t *) &Temp16)[0] | ((uint8_t *) &Temp16)[1]);
}

inline Bitboard duplicate(uint8_t checksum, const Bitboard& occlegoreye) 
{
	//uint16_t Temp16;
	//uint32_t Temp32;

	//((uint8_t *) &Temp16)[0] = ((uint8_t *) &checksum)[0];
	//((uint8_t *) &Temp16)[1] = ((uint8_t *) &checksum)[0];
	//((uint16_t *) &Temp32)[0] = Temp16;
	//((uint16_t *) &Temp32)[1] = Temp16;

	//return Bitboard(Temp32, Temp32, Temp32) &  occlegoreye;	 //返回checksum代表的马腿或者象眼的位棋盘


	uint16_t Temp16;
	uint32_t Temp32;
	uint64_t Temp64;

	((uint8_t *) &Temp16)[0] = ((uint8_t *) &checksum)[0];
	((uint8_t *) &Temp16)[1] = ((uint8_t *) &checksum)[0];
    ((uint16_t *) &Temp32)[0] = Temp16;
    ((uint16_t *) &Temp32)[1] = Temp16;
	((uint32_t *) &Temp64)[0] = Temp32;
	((uint32_t *) &Temp64)[1] = Temp32;

     return Bitboard(Temp64, Temp64) &  occlegoreye;
}
#endif