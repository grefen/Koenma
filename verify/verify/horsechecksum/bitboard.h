#ifndef bitboard_h
#define bitboard_h

class Bitboard
{
public:
	unsigned long Low, Mid, Hi;

public:
	Bitboard() 
	{
	}

	Bitboard(unsigned long Arg1, unsigned long Arg2 = 0, unsigned long Arg3 = 0) 
	{
		Low = Arg1;
		Mid = Arg2;
		Hi  = Arg3;
	}

	int operator !() const 
	{
		return !(Low || Mid || Hi);
	}

	int operator ==(const Bitboard &Arg) const 
	{
		return Low == Arg.Low && Mid == Arg.Mid && Hi == Arg.Hi;
	}

	bool operator != (const Bitboard &Arg) const
	{
        return Low != Arg.Low || Mid != Arg.Mid || Hi != Arg.Hi;
	}

	Bitboard operator ~() const 
	{
		return Bitboard(~Low, ~Mid, ~Hi);
	}

	Bitboard operator &(const Bitboard &Arg) const 
	{
		return Bitboard(Low & Arg.Low, Mid & Arg.Mid, Hi & Arg.Hi);
	}

	Bitboard operator |(const Bitboard &Arg) const 
	{
		return Bitboard(Low | Arg.Low, Mid | Arg.Mid, Hi | Arg.Hi);
	}

	Bitboard operator ^(const Bitboard &Arg) const 
	{
		return Bitboard(Low ^ Arg.Low, Mid ^ Arg.Mid, Hi ^ Arg.Hi);
	}

	Bitboard &operator &=(const Bitboard &Arg) 
	{
		Low &= Arg.Low;
		Mid &= Arg.Mid;
		Hi  &= Arg.Hi;
		return *this;
	}

	Bitboard &operator |=(const Bitboard &Arg) 
	{
		Low |= Arg.Low;
		Mid |= Arg.Mid;
		Hi  |= Arg.Hi;
		return *this;
	}

	Bitboard &operator ^=(const Bitboard &Arg) 
	{
		Low ^= Arg.Low;
		Mid ^= Arg.Mid;
		Hi  ^= Arg.Hi;
		return *this;
	}

	// Shift Operations

	Bitboard operator <<(int Arg) const
	{
		if (Arg < 0)
			return *this >> -Arg;
		else if (Arg == 0)
			return *this;
		else if (Arg < 32)
			return Bitboard(Low << Arg, Mid << Arg | Low >> (32 - Arg), Hi << Arg | Mid >> (32 - Arg));
		else if (Arg == 32)
			return Bitboard(0, Low, Mid);
		else if (Arg < 64)
			return Bitboard(0, Low << (Arg - 32), Mid << (Arg - 32) | Low >> (64 - Arg));
		else if (Arg == 64)
			return Bitboard(0, 0, Low);
		else if (Arg < 96)
			return Bitboard(0, 0, Low << (Arg - 64));
		else
			return Bitboard(0, 0, 0);
	}

	Bitboard operator >>(int Arg) const
	{
		if (Arg < 0)
			return *this << -Arg;
		else if (Arg == 0)
			return *this;
		else if (Arg < 32)
			return Bitboard(Low >> Arg | Mid << (32 - Arg), Mid >> Arg || Hi << (32 - Arg), Hi >> Arg);
		else if (Arg == 32)
			return Bitboard(Mid, Hi, 0);
		else if (Arg < 64)
			return Bitboard(Mid >> (Arg - 32) | Hi << (64 - Arg), Hi >> (Arg - 32), 0);
		else if (Arg == 64)
			return Bitboard(Hi, 0, 0);
		else if (Arg < 96)
			return Bitboard(Hi >> (Arg - 64), 0, 0);
		else
			return Bitboard(0, 0, 0);
	}

	Bitboard &operator <<=(int Arg)
	{
		if (Arg < 0) 
		{
			*this >>= -Arg;
		} 
		else if (Arg == 0) 
		{
		} 
		else if (Arg < 32) 
		{
			Hi <<= Arg;
			Hi |= Mid >> (32 - Arg);
			Mid <<= Arg;
			Mid |= Low >> (32 - Arg);
			Low <<= Arg;
		} 
		else if (Arg == 32) 
		{
			Hi = Mid;
			Mid = Low;
			Low = 0;
		} 
		else if (Arg < 64) 
		{
			Hi = Mid << (Arg - 32);
			Hi |= Low >> (64 - Arg);
			Mid = Low << (Arg - 32);
			Low = 0;
		} 
		else if (Arg == 64) 
		{
			Hi  = Low;
			Mid = 0;
			Low = 0;
		} 
		else if (Arg < 96) 
		{
			Hi = Low << (Arg - 64);
			Mid = 0;
			Low = 0;
		} 
		else 
		{
			Hi  = 0;
			Mid = 0;
			Low = 0;		
		}
		return *this;
	}

	Bitboard &operator >>=(int Arg)
	{
		if (Arg < 0) 
		{
			*this <<= -Arg;
		} 
		else if (Arg == 0) 
		{
		} 
		else if (Arg < 32) 
		{
			Low >>= Arg;
			Low |= Mid << (32 - Arg);
			Mid >>= Arg;
			Mid |= Hi << (32 - Arg);
			Hi >>= Arg;
		} 
		else if (Arg == 32) 
		{
			Low = Mid;
			Mid = Hi;
			Hi = 0;
		} 
		else if (Arg < 64) 
		{
			Low = Mid >> (Arg - 32);
			Low |= Hi << (64 - Arg);
			Mid = Hi >> (Arg - 32);
			Hi = 0;
		} 
		else if (Arg == 64) 
		{
			Low = Hi;
			Mid = 0;
			Hi = 0;
		} 
		else if (Arg < 96) 
		{
			Low = Hi >> (Arg - 64);
			Mid = 0;
			Hi = 0;
		} 
		else 
		{
			Low = 0;
			Mid = 0;
			Hi = 0;
		}
		return *this;
	}

	void print()
	{
       int x = 0;
	   int n = 1;
	   for (int i = 0; i < 32; ++i)
	   {
		   if (n&Low)
		   {
			   printf("%d",1);
		   }
		   else
		   {
			    printf("%d",0);
		   }
		   n = n<<1;

		   if ((x+1)%9 == 0 )
		   {
			   printf("\n");
		   }
		   x++;
	   }

	   n = 1;
	   for (int i = 0; i < 32; ++i)
	   {
		   if (n&Mid)
		   {
			   printf("%d",1);
		   }
		   else
		   {
			   printf("%d",0);
		   }
		   n = n<<1;

		   if ((x+1)%9 == 0 )
		   {
			   printf("\n");
		   }
		   x++;
	   }

	   n = 1;
	   for (int i = 0; i < 32; ++i)
	   {
		   if (n&Hi)
		   {
			   printf("%d",1);
		   }
		   else
		   {
			   printf("%d",0);
		   }
		   n = n<<1;

		   if ((x+1)%9 == 0 )
		   {
			   printf("\n");
		   }
		   x++;
	   }

	    printf("\n\n");

	}
};



#endif