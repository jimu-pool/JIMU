//
//  OTExtension.h
//  
//
//  Created by Sourgroup on 7/18/17.
//
//

#ifndef OTExtension_h
#define OTExtension_h

#include <stdio.h>
#include "wmmintrin.h"
#include "aes.h"
#include <math.h>
#include "util.h"
#include <emmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <smmintrin.h>
#include <stdlib.h>

#define __xor _mm_xor_si128
typedef struct{
    unsigned char __owner;
    unsigned char __value;
    block __key;
}Bit;
block __delta;

block OTdelta;
block bitpad[128];
void RandomOTSetup(block __delta);
void RandomOTSend(block *__dst,int n);
void RandomOTReceive(block *__dst,int n,unsigned char *choice);

block RandomSeed;
block RandomCount;
block Pad;


static inline void mul128(__m128i a, __m128i b, __m128i *res1, __m128i *res2)
{
    __m128i tmp3, tmp4, tmp5, tmp6;
    
    tmp3 = _mm_clmulepi64_si128(a, b, 0x00);
    tmp4 = _mm_clmulepi64_si128(a, b, 0x10);
    tmp5 = _mm_clmulepi64_si128(a, b, 0x01);
    tmp6 = _mm_clmulepi64_si128(a, b, 0x11);
    
    tmp4 = _mm_xor_si128(tmp4, tmp5);
    tmp5 = _mm_slli_si128(tmp4, 8);
    tmp4 = _mm_srli_si128(tmp4, 8);
    tmp3 = _mm_xor_si128(tmp3, tmp5);
    tmp6 = _mm_xor_si128(tmp6, tmp4);
    // initial mul now in tmp3, tmp6
    *res1 = tmp3;
    *res2 = tmp6;
}

typedef struct{
    block Seed[128];
    AES_KEY extSeed[128];
    block coeseed;
    AES_KEY extcoeseed;
    block otdelta;
    unsigned char otdeltac[128];
    uint64_t otcount;
}OTsetSend;
void otsetupsend(block delta,OTsetSend  *ot);
void otextsend(uint64_t n,block *out,OTsetSend *ot);

typedef struct{
    block Seed[128][2];
    AES_KEY extSeed[128][2];
    block coeseed,randominputseed;
    AES_KEY extcoeseed;
    AES_KEY extrandominputseed;

    uint64_t otcount;
}OTsetReceive;

void otsetupreceive(OTsetReceive  *ot);
void otextreceive(uint64_t n,block *in,block *out, OTsetReceive *ot);


OTsetSend OTS;
OTsetReceive OTR;
//========================

block Seed[128];
AES_KEY extSeed[128];
block coeseed;
AES_KEY extcoeseed;

block otdelta;
unsigned char otdeltac[128];
#endif /* OTExtension_h */
