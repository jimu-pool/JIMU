//
//  WireLabelOperation.h
//  
//
//  Created by Sourgroup on 9/13/16.
//
//

#ifndef WireLabelOperation_h
#define WireLabelOperation_h

#include <stdio.h>
#include <gcrypt.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <smmintrin.h>
#include <stdint.h>
#include "aes.h"
#include "RandomMatrix.h"
#include "RSEncoding.h"
#include "PermutationOperation.h"
#define WLN 88
#define WLL 48
#define WLW 32
typedef struct{
    __m256i L0;
    __m128i L1;
}WireLabel;
typedef struct{
    __m256i H0;
    __m128i H1;
}WLabelPad;

typedef struct{
    WireLabel L;
    WLabelPad H;
}WLabelHash;

#ifdef ALICE
typedef struct{
    PmtBit P;
    WireLabel L;
}Wire;
#endif

#ifdef BOB
typedef struct{
    PBitHash P;
    WLabelHash L;
}Wire;
typedef struct{
    Wire W;
    WireLabel L;
}WireE;
#endif

#define _mm256_set_m128i(v0, v1)  _mm256_insertf128_si256(_mm256_castsi128_si256(v1), (v0), 1)
#define m256Xor(h1,h2)\
({\
    /*_mm256_set_m128(_mm_xor_si128(_mm256_extractf128_si256(h1,1),_mm256_extractf128_si256(h2,1)),_mm_xor_si128(_mm256_extractf128_si256(h1,0),_mm256_extractf128_si256(h2,0)));*/\
    /*_mm256_xor_si256(h1,h2);*/\
    (__m256i)_mm256_xor_ps((__m256)h1,(__m256)h2);\
})

#define WLPXor(h1,h2)\
({\
    WLabelPad WLPXor_result;\
    (WLPXor_result).H0=m256Xor((h1).H0,(h2).H0);\
    (WLPXor_result).H1=_mm_xor_si128((h1).H1,(h2).H1);\
    WLPXor_result;\
})

#define WLPConstruct( tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39)\
({\
    WLabelPad WLPConstruct_result;\
    WLPConstruct_result.H0=_mm256_set_epi8(tmp31,tmp30,tmp29,tmp28,tmp27,tmp26,tmp25,tmp24,tmp23,tmp22,tmp21,tmp20,tmp19,tmp18,tmp17,tmp16,tmp15,tmp14,tmp13,tmp12,tmp11,tmp10,tmp9,tmp8,tmp7,tmp6,tmp5,tmp4,tmp3,tmp2,tmp1,tmp0);\
    WLPConstruct_result.H1=_mm_set_epi8(0,0,0,0,0,0,0,0,tmp39,tmp38,tmp37,tmp36,tmp35,tmp34,tmp33,tmp32);\
    WLPConstruct_result;\
})

#define WLConstruct( tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47)\
({\
    WireLabel WLConstruct_result;\
    WLConstruct_result.L0=_mm256_set_epi8(tmp31,tmp30,tmp29,tmp28,tmp27,tmp26,tmp25,tmp24,tmp23,tmp22,tmp21,tmp20,tmp19,tmp18,tmp17,tmp16,tmp15,tmp14,tmp13,tmp12,tmp11,tmp10,tmp9,tmp8,tmp7,tmp6,tmp5,tmp4,tmp3,tmp2,tmp1,tmp0);\
    WLConstruct_result.L1=_mm_set_epi8(tmp47,tmp46,tmp45,tmp44,tmp43,tmp42,tmp41,tmp40,tmp39,tmp38,tmp37,tmp36,tmp35,tmp34,tmp33,tmp32);\
    WLConstruct_result;\
})

#define WLLSB(L)\
({\
    ((unsigned char)_mm_extract_epi8(L.L1,0))%2;\
})

#define WLabelXor(l1, l2)\
({\
    WireLabel WLabelXor_result;\
    WLabelXor_result.L0=m256Xor(l1.L0,l2.L0);\
    WLabelXor_result.L1=_mm_xor_si128(l1.L1,l2.L1);\
    WLabelXor_result;\
})

#define WLHXor(h1,h2)\
({\
    WLabelHash WLHXor_result;\
    WLHXor_result.L=WLabelXor(h1.L,h2.L);\
    WLHXor_result.H=WLPXor(h1.H,h2.H);\
    WLHXor_result;\
})


#define PsendWL(a)\
({\
psend((unsigned char *)&(a),WLL);\
})
#define PsendWLP(a)\
({\
psend((unsigned char *)&(a),48);\
})
#define PreceiveWL(a)\
({\
preceive((unsigned char *)&(a),WLL);\
})
#define PreceiveWLP(a)\
({\
preceive((unsigned char *)&(a),48);\
})
WLabelPad ** RSWLMatrix;
WLabelPad ** RSWLTable (int m,int n,int w);
static inline WLabelPad WLPad (WireLabel l);
WireLabel LabelRanGen();
WLabelPad WLPad (WireLabel l);
void WLprint(WireLabel L);
void WLPprint(WLabelPad L);
#ifdef ALICE
static inline void PrintWL(WireLabel L)
{
    WLprint(L);
    WLPprint(WLPad(L));
    printf("\n");
}
#endif
#ifdef BOB
static inline void PrintWL(WLabelHash L)
{
    WLprint(L.L);
    WLPprint(L.H);
    WLPprint(WLPad(L.L));
    printf("\n");
}
#endif
static inline block RMEnc(WireLabel l,block **RMTable)
{
    block result;
    uint8_t *tmp;
    tmp=(uint8_t *)&(l);
    result=RMTable[0][tmp[0]];
    int i;
    for (i=1;i<32;i++)
        result=_mm_xor_si128(result,RMTable[i][tmp[i]]);
    result=_mm_slli_epi64(result,1);
    return result;
}

static inline WLabelPad WLPad (WireLabel l)
{
    WLabelPad result;
    int i;
    uint8_t *tmp;
    tmp=(uint8_t *)&(l.L0);
    result=RSWLMatrix[0][tmp[0]];
    for(i=1;i<32;i++)
    {
        result=WLPXor (result, RSWLMatrix[i][tmp[i]]);
    }
    tmp=(uint8_t *)&(l.L1);
    for(i=0;i<16;i++)
    {
        result=WLPXor (result, RSWLMatrix[i+32][tmp[i]]);
    }

    return result;
}
void freeRSWLTable();
#endif /* WireLabelOperation_h */
