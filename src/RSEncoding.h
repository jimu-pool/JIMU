//
//  RSEncoding.h
//  
//
//  Created by Sourgroup on 9/13/16.
//
//

#ifndef RSEncoding_h
#define RSEncoding_h

#include <stdio.h>
#include "galois.h"
#include <gcrypt.h>
#include <emmintrin.h>
#include <immintrin.h>
#include "block.h"
#include <math.h>
block SHL128(block v,int n);
block SHR128(block v,int n);
unsigned char ** RSMatrix(int m,int n,int w);
#define SHL128(v, n) \
({ \
__m128i v1, v2; \
\
if ((n) >= 64) \
{ \
v1 = _mm_slli_si128(v, 8); \
v1 = _mm_slli_epi64(v1, (n) - 64); \
} \
else \
{ \
v1 = _mm_slli_epi64(v, n); \
v2 = _mm_slli_si128(v, 8); \
v2 = _mm_srli_epi64(v2, 64 - (n)); \
v1 = _mm_or_si128(v1, v2); \
} \
v1; \
})

#define SHR128(v, n) \
({ \
__m128i v1, v2; \
\
if ((n) >= 64) \
{ \
v1 = _mm_srli_si128(v, 8); \
v1 = _mm_srli_epi64(v1, (n) - 64); \
} \
else \
{ \
v1 = _mm_srli_epi64(v, n); \
v2 = _mm_srli_si128(v, 8); \
v2 = _mm_slli_epi64(v2, 64 - (n)); \
v1 = _mm_or_si128(v1, v2); \
} \
v1; \
})
#endif /* RSEncoding_h */
