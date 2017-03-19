#include "possiblemoves.h"

// ###########################
// ## If there is only AVX2 ##
// ###########################
#if defined(HAS_AVX2) && !defined(HAS_AVX152)
	uint64_t PossibleMoves(const uint64_t P, const uint64_t O)
	{
		// 10 x SHIFT, 11 x AND, 10 x OR, 1 x NOT
		// = 32 OPs
		
		// 1 x AND
		const __m256i PP = _mm256_set1_epi64x(P);
		const __m256i maskO = _mm256_set1_epi64x(O) & _mm256_set_epi64x(0x7E7E7E7E7E7E7E7EULL, 0x00FFFFFFFFFFFF00ULL, 0x007E7E7E7E7E7E00ULL, 0x007E7E7E7E7E7E00ULL);
		const __m256i shift1 = _mm256_set_epi64x(1,  8,  7,  9);
		const __m256i shift2 = shift1 + shift1;
		__m256i mask1, mask2;
		__m256i flip1, flip2;

		// 2 x SHIFT, 2 x AND
		flip1 = maskO & _mm256_sllv_epi64(PP, shift1);
		flip2 = maskO & _mm256_srlv_epi64(PP, shift1);

		// 2 x SHIFT, 2 x AND, 2 x OR
		flip1 |= maskO & _mm256_sllv_epi64(flip1, shift1);
		flip2 |= maskO & _mm256_srlv_epi64(flip2, shift1);

		// 2 x SHIFT, 1 x AND
		mask1 = maskO & _mm256_sllv_epi64(maskO, shift1);
		mask2 = _mm256_srlv_epi64(mask1, shift1);

		// 2 x SHIFT, 2 x AND, 2 x OR
		flip1 |= mask1 & _mm256_sllv_epi64(flip1, shift2);
		flip2 |= mask2 & _mm256_srlv_epi64(flip2, shift2);

		// 2 x SHIFT, 2 x AND, 2 x OR
		flip1 |= mask1 & _mm256_sllv_epi64(flip1, shift2);
		flip2 |= mask2 & _mm256_srlv_epi64(flip2, shift2);

		// 2 x SHIFT
		flip1 = _mm256_sllv_epi64(flip1, shift1);
		flip2 = _mm256_srlv_epi64(flip2, shift1);

		// 2 x OR
		flip1 |= flip2;
		__m128i flip = _mm256_castsi256_si128(flip1) | _mm256_extracti128_si256(flip1, 1);
		
		// 1 x NOT, 2 x OR, 1 x AND
		return ~(P | O) & (_mm_extract_epi64(flip, 0) | _mm_extract_epi64(flip, 1));
	}

	
	__m256i PossibleMoves(const __m256i P, const __m256i O)
	{
		// 48 x SHIFT, 42 x AND, 32 x OR, 1 x NOT
		// = 123 OPs
		__m256i mask1, mask2, mask3, mask4, mask5, mask6, mask7, mask8;
		__m256i flip1, flip2, flip3, flip4, flip5, flip6, flip7, flip8;

		// 1 x AND
		const __m256i maskO = O & _mm256_set1_epi64x(0x7E7E7E7E7E7E7E7EULL);

		// 8 x SHIFT, 8 x AND
		flip1 = maskO & _mm256_slli_epi64(P, 1);
		flip2 = maskO & _mm256_srli_epi64(P, 1);
		flip3 =     O & _mm256_slli_epi64(P, 8);
		flip4 =     O & _mm256_srli_epi64(P, 8);
		flip5 = maskO & _mm256_slli_epi64(P, 7);
		flip6 = maskO & _mm256_srli_epi64(P, 7);
		flip7 = maskO & _mm256_slli_epi64(P, 9);
		flip8 = maskO & _mm256_srli_epi64(P, 9);

		// 8 x SHIFT, 8 x AND, 8 x OR
		flip1 |= maskO & _mm256_slli_epi64(flip1, 1);
		flip2 |= maskO & _mm256_srli_epi64(flip2, 1);
		flip3 |=     O & _mm256_slli_epi64(flip3, 8);
		flip4 |=     O & _mm256_srli_epi64(flip4, 8);
		flip5 |= maskO & _mm256_slli_epi64(flip5, 7);
		flip6 |= maskO & _mm256_srli_epi64(flip6, 7);
		flip7 |= maskO & _mm256_slli_epi64(flip7, 9);
		flip8 |= maskO & _mm256_srli_epi64(flip8, 9);

		// 8 x SHIFT, 8 x AND
		mask1 = maskO & _mm256_slli_epi64(maskO, 1); mask2 = _mm256_srli_epi64(mask1, 1);
		mask3 =     O & _mm256_slli_epi64(    O, 8); mask4 = _mm256_srli_epi64(    O, 8);
		mask5 = maskO & _mm256_slli_epi64(maskO, 7); mask6 = _mm256_srli_epi64(maskO, 7);
		mask7 = maskO & _mm256_slli_epi64(maskO, 9); mask8 = _mm256_srli_epi64(maskO, 9);

		// 8 x SHIFT, 8 x AND, 8 x OR
		flip1 |= mask1 & _mm256_slli_epi64(flip1,  2);
		flip2 |= mask2 & _mm256_srli_epi64(flip2,  2);
		flip3 |= mask3 & _mm256_slli_epi64(flip3, 16);
		flip4 |= mask4 & _mm256_srli_epi64(flip4, 16);
		flip5 |= mask5 & _mm256_slli_epi64(flip5, 14);
		flip6 |= mask6 & _mm256_srli_epi64(flip6, 14);
		flip7 |= mask7 & _mm256_slli_epi64(flip7, 18);
		flip8 |= mask8 & _mm256_srli_epi64(flip8, 18);

		// 8 x SHIFT, 8 x AND, 8 x OR
		flip1 |= mask1 & _mm256_slli_epi64(flip1,  2);
		flip2 |= mask2 & _mm256_srli_epi64(flip2,  2);
		flip3 |= mask3 & _mm256_slli_epi64(flip3, 16);
		flip4 |= mask4 & _mm256_srli_epi64(flip4, 16);
		flip5 |= mask5 & _mm256_slli_epi64(flip5, 14);
		flip6 |= mask6 & _mm256_srli_epi64(flip6, 14);
		flip7 |= mask7 & _mm256_slli_epi64(flip7, 18);
		flip8 |= mask8 & _mm256_srli_epi64(flip8, 18);

		// 8 x SHIFT
		flip1 = _mm256_slli_epi64(flip1, 1);
		flip2 = _mm256_srli_epi64(flip2, 1);
		flip3 = _mm256_slli_epi64(flip3, 8);
		flip4 = _mm256_srli_epi64(flip4, 8);
		flip5 = _mm256_slli_epi64(flip5, 7);
		flip6 = _mm256_srli_epi64(flip6, 7);
		flip7 = _mm256_slli_epi64(flip7, 9);
		flip8 = _mm256_srli_epi64(flip8, 9);

		// 8 x SHIFT, 1 x AND, 8 x OR, 1 x NOT
		return ~(P | O) & (flip1 | flip2 | flip3 | flip4 | flip5 | flip6 | flip7);
	}
#endif