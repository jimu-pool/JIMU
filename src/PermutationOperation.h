//
//  PermutationOperation.h
//  
//
//  Created by Sourgroup on 9/15/16.
//
//

#ifndef PermutationOperation_h
#define PermutationOperation_h

#include <stdio.h>
#include <gcrypt.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <stdint.h>
#include "aes.h"
#include "RandomMatrix.h"
#include "RSEncoding.h"
#define PBN 44
#define PBL 21
#define PBW 20
typedef __m256i PmtBit;
typedef __m256i PBitPad;
typedef struct{
    PmtBit B;
    PBitPad H;
}PBitHash;
#define m256Xor(h1,h2)\
({\
/*_mm256_set_m128(_mm_xor_si128(_mm256_extractf128_si256(h1,1),_mm256_extractf128_si256(h2,1)),_mm_xor_si128(_mm256_extractf128_si256(h1,0),_mm256_extractf128_si256(h2,0)));*/\
/*_mm256_xor_si256(h1,h2);*/\
(__m256i)_mm256_xor_ps((__m256)h1,(__m256)h2);\
})
#define PBitXor(b1,b2)\
({\
    m256Xor(b1,b2);\
})
#define PBPXor(l1,l2)\
({\
    m256Xor(l1,l2);\
})
#define PBConstruct(tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,tmp10,tmp11,tmp12,tmp13,tmp14,tmp15,tmp16,tmp17,tmp18,tmp19,tmp20)\
({\
   /* _mm_set_epi8(tmp20,tmp19*4+tmp18/16,(tmp18%16)*16+tmp17%16,(tmp17/16)*64+tmp16,tmp15*4+tmp14/16,(tmp14%16)*16+tmp13%16,(tmp13/16)*64+tmp12,tmp11*4+tmp10/16,(tmp10%16)*16+tmp9%16,(tmp9/16)*64+tmp8,tmp7*4+tmp6/16,(tmp6%16)*16+tmp5%16,(tmp5/16)*64+tmp4,tmp3*4+tmp2/16,(tmp2%16)*16+tmp1%16,(tmp1/16)*64+tmp0);*/\
    _mm256_set_epi8(0,0,0,0,0,0,0,0,0,0,0,tmp20,tmp19,tmp18,tmp17,tmp16,tmp15,tmp14,tmp13,tmp12,tmp11,tmp10,tmp9,tmp8,tmp7,tmp6,tmp5,tmp4,tmp3,tmp2,tmp1,tmp0);\
})
#define PBPConstruct(tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,tmp10,tmp11,tmp12,tmp13,tmp14,tmp15,tmp16,tmp17,tmp18,tmp19,tmp20,tmp21,tmp22)\
({\
    /*PBitPad PBPConstruct_result;\
    PBPConstruct_result.a=_mm_set_epi8(tmp20,tmp19*4+tmp18/16,(tmp18%16)*16+tmp17%16,(tmp17/16)*64+tmp16,tmp15*4+tmp14/16,(tmp14%16)*16+tmp13%16,(tmp13/16)*64+tmp12,tmp11*4+tmp10/16,(tmp10%16)*16+tmp9%16,(tmp9/16)*64+tmp8,tmp7*4+tmp6/16,(tmp6%16)*16+tmp5%16,(tmp5/16)*64+tmp4,tmp3*4+tmp2/16,(tmp2%16)*16+tmp1%16,(tmp1/16)*64+tmp0);\
    PBPConstruct_result.b[0]=tmp21;\
    PBPConstruct_result.b[1]=tmp22;\
    PBPConstruct_result;\*/\
    _mm256_set_epi8(0,0,0,0,0,0,0,0,0,tmp22,tmp21,tmp20,tmp19,tmp18,tmp17,tmp16,tmp15,tmp14,tmp13,tmp12,tmp11,tmp10,tmp9,tmp8,tmp7,tmp6,tmp5,tmp4,tmp3,tmp2,tmp1,tmp0);\
})
#define PBHXor(b1,b2)\
({\
    PBitHash PBHXor_result;\
    PBHXor_result.B=PBitXor(b1.B,b2.B);\
    PBHXor_result.H=PBPXor(b1.H,b2.H);\
    PBHXor_result;\
})
#define PsendPB(b)\
({\
   psend((unsigned char *)&b,32);\
})
#define PsendPBP(b)\
({\
    psend((unsigned char *)&b,32); \
})
#define PreceivePB(b)\
({\
    preceive((unsigned char *)&b,32);\
})
#define PreceivePBP(b)\
({\
    preceive((unsigned char *)&(b),32); \
})
PBitPad** RSPBMatrix;
PmtBit BitRanGen();
//PBitPad PBPXor(PBitPad l1, PBitPad l2);
PBitPad** RSPBTable (int m,int n,int w);
PBitPad PBPad (PmtBit b);
static inline unsigned char PBDec(PmtBit B)
{
    int i;
    unsigned char tmp=0,tmp0=0;
    uint8_t *t;
    t=(uint8_t *)&B;
    for(i=0;i<16;i++)
        tmp^=t[i];
    for(i=0;i<8;i++)
    {
        tmp0^=tmp;
        tmp=tmp>>1;
    }
    return tmp0%2;
}
void PBprint(PmtBit L);
void PBPprint(PBitPad L);
#ifdef ALICE
static inline void PrintPB(PmtBit L)
{
    PBprint(L);
    PBPprint(PBPad(L));
    printf("\n");
}
#endif
#ifdef BOB
static inline void PrintPB(PBitHash L)
{
    PBprint(L.B);
    PBPprint(L.H);
    PBprint(PBPad(L.B));
    printf("\n");
}
#endif

void freeRSPBTable();
//PBitPad PBPConstruct (unsigned char *temp);
//PmtBit PBConstruct(unsigned char *temp);
#endif /* PermutationOperation_h */
