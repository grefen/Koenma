#ifndef checksum_h
#define checksum_h
#include "bitboard.h"

//棋盘经过三次折叠，折叠成8数，用途是作为马腿和相眼的hash key
//一共有256种情况
inline int checksum(const Bitboard& board) 
{
	unsigned long  Temp32 = board.Low | board.Mid | board.Hi;
	unsigned short Temp16 = ((unsigned short *) &Temp32)[0] | ((unsigned short *) &Temp32)[1];
	return (int) (((unsigned char *) &Temp16)[0] | ((unsigned char *) &Temp16)[1]);
}


//展开校验和，其作用与checksum()相反。其返回值是马腿或象眼的位棋盘，occ其位置所有的马腿或象眼
//这个函数只在马和象的着法预产生函数种应用
//在预产生招法那里，通过这个函数计算checksum与马腿或相眼位棋盘对应关系
inline Bitboard duplicate(int checksum, const Bitboard& occ) 
{
	unsigned short Temp16;
	unsigned long Temp32;

	((unsigned char *) &Temp16)[0] = ((unsigned char *) &checksum)[0];
	((unsigned char *) &Temp16)[1] = ((unsigned char *) &checksum)[0];
	((unsigned short *) &Temp32)[0] = Temp16;
	((unsigned short *) &Temp32)[1] = Temp16;

	return Bitboard(Temp32, Temp32, Temp32) &  occ;	 //返回checksum代表的马腿或者象眼的位棋盘
}

#endif