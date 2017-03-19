#include "macros_hell.h"
#include "possiblemoves.h"
#include "flip_fast.h"
#include "helpers.h"
#include "gtest/gtest.h"

TEST (PossibleMovesTest, HasMoves)
{
	for (unsigned int i = 0; i < 1000000; i++)
	{
		CPosition pos;
		pos.MakeRandom();
		
		uint64_t empties = ~(pos.P | pos.O);
		bool hasMoves = false;
		
		while (empties)
		{
			unsigned long move = BitScanLSB(empties);
			RemoveLSB(empties);
			if (flip(pos.P, pos.O, move) != 0ULL)
			{
				hasMoves = true;
				break;
			}
		}
		
		ASSERT_EQ (HasMoves(pos.P, pos.O), hasMoves);
	}
}

TEST (PossibleMovesTest, PossibleMoves)
{
	for (unsigned int i = 0; i < 1000000; i++)
	{
		CPosition pos;
		pos.MakeRandom();
		
		uint64_t empties = ~(pos.P | pos.O);
		uint64_t possibleMoves = 0ULL;
		
		while (empties)
		{
			unsigned long move = BitScanLSB(empties);
			if (flip(pos.P, pos.O, move) != 0ULL)
				possibleMoves |= GetLSB(empties);
			RemoveLSB(empties);
		}
		
		ASSERT_EQ (PossibleMoves(pos.P, pos.O), possibleMoves);
	}
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);	
	return RUN_ALL_TESTS();
}