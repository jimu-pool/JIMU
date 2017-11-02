//
//  BatchGateGeneration.h
//  
//
//  Created by Sourgroup on 9/10/17.
//
//

#ifndef BatchGateGeneration_h
#define BatchGateGeneration_h
#include "IHash.h"
typedef struct{
    IHashSet IHash;
    uint64_t GateCount;
    AES_KEY AesKey;
    block **RMTable;
#ifdef ALICE
    WireLabel Delta;
#endif

#ifdef BOB
    WLabelHash Delta;
#endif

}GateSet;
void GateSetup(GateSet *Gate_Set,block key);
#ifdef ALICE
typedef struct{
    uint64_t *GarbleID;
    PmtBit *BLP,*BRP,*BOP;
    WireLabel *BL,*BR,*BO,*Mask;
    WireLabel *TG,*TE;
}BatchGate;
#endif
#ifdef BOB
typedef struct{
    uint64_t *GarbleID;
    PBitHash *BLPH,*BRPH,*BOPH;
    WLabelHash *BLH,*BRH,*BOH;
    WireLabel *Mask;
    WireLabel *TG,*TE;
}BatchGate;
#endif

typedef struct{
    uint64_t ID;
    BatchGate* BG;
}BGRef;

#ifdef ALICE
static inline void CopyBatchGate(BGRef *dst,BGRef *src)
{
    dst->BG->GarbleID[dst->ID]=src->BG->GarbleID[src->ID];
    dst->BG->BLP[dst->ID]=src->BG->BLP[src->ID];
    dst->BG->BRP[dst->ID]=src->BG->BRP[src->ID];
    dst->BG->BOP[dst->ID]=src->BG->BOP[src->ID];
    dst->BG->BL[dst->ID]=src->BG->BL[src->ID];
    dst->BG->BR[dst->ID]=src->BG->BR[src->ID];
    dst->BG->BO[dst->ID]=src->BG->BO[src->ID];
    dst->BG->Mask[dst->ID]=src->BG->Mask[src->ID];
    dst->BG->TG[dst->ID]=src->BG->TG[src->ID];
    dst->BG->TE[dst->ID]=src->BG->TE[src->ID];
}
static inline void MallocBatchGate(BatchGate *dst, uint64_t n)
{
    if(n%16!=0) error("Malloc Gate Number Wrong!");
    dst->BLP=_mm_malloc(sizeof(PmtBit)*3*n,32);
    dst->BRP=&(dst->BLP[n]);
    dst->BOP=&(dst->BLP[2*n]);
    
    dst->BL=_mm_malloc(sizeof(WireLabel)*6*n,32);
    dst->BR=&(dst->BL[n]);
    dst->BO=&(dst->BL[2*n]);
    dst->Mask=&(dst->BL[3*n]);
    dst->TG=&(dst->BL[4*n]);
    dst->TE=&(dst->BL[5*n]);
    dst->GarbleID=malloc(sizeof(uint64_t)*n);
}
static inline void FreeBatchGate(BatchGate *tmp)
{
    _mm_free(tmp->BLP);
    _mm_free(tmp->BL);
    free(tmp->GarbleID);
}
#endif

#ifdef BOB
static inline void CopyBatchGate(BGRef *dst, BGRef *src)
{
    dst->BG->GarbleID[dst->ID]=src->BG->GarbleID[src->ID];
    dst->BG->BLPH[dst->ID]=src->BG->BLPH[src->ID];
    dst->BG->BRPH[dst->ID]=src->BG->BRPH[src->ID];
    dst->BG->BOPH[dst->ID]=src->BG->BOPH[src->ID];
    dst->BG->BLH[dst->ID]=src->BG->BLH[src->ID];
    dst->BG->BRH[dst->ID]=src->BG->BRH[src->ID];
    dst->BG->BOH[dst->ID]=src->BG->BOH[src->ID];
    dst->BG->Mask[dst->ID]=src->BG->Mask[src->ID];
    dst->BG->TG[dst->ID]=src->BG->TG[src->ID];
    dst->BG->TE[dst->ID]=src->BG->TE[src->ID];
}

static inline void MallocBatchGate(BatchGate *dst, uint64_t n)
{
    if(n%16!=0) error("Malloc Gate Number Wrong!");
    dst->BLPH=_mm_malloc(sizeof(PBitHash)*3*n,32);
    dst->BRPH=&(dst->BLPH[n]);
    dst->BOPH=&(dst->BLPH[2*n]);
    
    dst->BLH=_mm_malloc(sizeof(WLabelHash)*3*n,32);
    dst->BRH=&(dst->BLH[n]);
    dst->BOH=&(dst->BLH[2*n]);
    
    dst->Mask=_mm_malloc(sizeof(WireLabel)*3*n,32);
    dst->TG=&(dst->Mask[1*n]);
    dst->TE=&(dst->Mask[2*n]);
    
    dst->GarbleID=malloc(sizeof(uint64_t)*n);
}
static inline void FreeBatchGate(BatchGate *tmp)
{
    _mm_free(tmp->BLPH);
    _mm_free(tmp->BLH);
    _mm_free(tmp->Mask);
    free(tmp->GarbleID);
}
#endif
static inline WireLabel Garbling (WireLabel l, uint64_t j,GateSet *Gate_Set)
{
    //    return l;
    block a[3],b[3];
    block c1,c2,c3;
    c1=_mm_set_epi64x(0,3*j);
    c2=_mm_set_epi64x(0,3*j+1);
    c3=_mm_set_epi64x(0,3*j+2);
    
    a[0]=RMEnc(l,Gate_Set->RMTable);
    
    a[2]=_mm_xor_si128(a[0],c3);
    a[1]=_mm_xor_si128(a[0],c2);
    a[0]=_mm_xor_si128(a[0],c1);
    
    b[0]=a[0];
    b[1]=a[1];
    b[2]=a[2];
    
    AES_ecb_encrypt_blks(a,3,&(Gate_Set->AesKey));
    
    a[0]=_mm_xor_si128(a[0],b[0]);
    a[1]=_mm_xor_si128(a[1],b[1]);
    a[2]=_mm_xor_si128(a[2],b[2]);
    
    WireLabel result;
    result.L0=_mm256_set_m128i (a[0],a[1]);
    result.L1=a[2];
    return result;
}


WireLabel BatchGateEval(BGRef *G, WireLabel L1, WireLabel L2,GateSet *Gate_Set);
void BatchGateGeneration(BatchGate *dst, uint64_t n,GateSet *Gate_Set);
void BatchGateCheck (BGRef *G,unsigned char a,unsigned char b,GateSet *Gate_Set);
#endif /* BatchGateGeneration_h */
