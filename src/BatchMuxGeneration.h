//
//  BatchMuxGeneration.h
//  
//
//  Created by Sourgroup on 9/13/17.
//
//

#ifndef BatchMuxGeneration_h
#define BatchMuxGeneration_h
#include "BatchGateGeneration.h"
#ifdef ALICE
typedef WireLabel WL;
typedef PmtBit PB;
typedef Wire W;
#endif
#ifdef BOB
typedef WLabelHash WL;
typedef PBitHash PB;
typedef WireE W;
#endif
typedef struct{
    uint64_t L;
    uint64_t *MuxID;
    PB *BLP,*BRP,*BOP;
    WL *BL,*BR,*BO;
    WireLabel *TG;
    WireLabel *TE;
    WireLabel *Mask;
}BatchMuxCore;
typedef struct{
    BatchMuxCore *BMC;
    uint64_t ID;
}BMCRef;
static inline void BMCCopy(BMCRef *dst,BMCRef *src)
{
    uint64_t i;
    uint64_t L=dst->BMC->L;
    for(i=0;i<L;i++)
    {
        dst->BMC->BLP[dst->ID*L+i]=src->BMC->BLP[(src->ID)*L+i];
        dst->BMC->BL[dst->ID*L+i]=src->BMC->BL[(src->ID)*L+i];
        dst->BMC->BOP[dst->ID*L+i]=src->BMC->BOP[(src->ID)*L+i];
        dst->BMC->BO[dst->ID*L+i]=src->BMC->BO[(src->ID)*L+i];
        dst->BMC->TG[dst->ID*L+i]=src->BMC->TG[(src->ID)*L+i];
        dst->BMC->TE[dst->ID*L+i]=src->BMC->TE[(src->ID)*L+i];
        dst->BMC->Mask[dst->ID*L+i]=src->BMC->Mask[(src->ID)*L+i];
    }
    dst->BMC->BR[dst->ID]=src->BMC->BR[src->ID];
    dst->BMC->BRP[dst->ID]=src->BMC->BRP[src->ID];
    dst->BMC->MuxID[dst->ID]=src->BMC->MuxID[src->ID];
}
static inline void MallocBatchMuxCore(BatchMuxCore *BMC,uint64_t L,uint64_t n)
{
    BMC->L=L;
    if (n%16!=0)error("Malloc Muxcore number error\n");
    BMC->BLP=_mm_malloc(sizeof(PB)*(2*L+1)*n,32);
    BMC->BL=_mm_malloc(sizeof(WL)*(2*L+1)*n,32);
    BMC->TG=_mm_malloc(sizeof(WireLabel)*(3*L)*n,32);
    
    BMC->BRP=&(BMC->BLP[n*L]);
    BMC->BR=&(BMC->BL[n*L]);
    
    BMC->BOP=&(BMC->BLP[n*L+n]);
    BMC->BO=&(BMC->BL[n*L+n]);
    
    BMC->TE=&(BMC->TG[n*L]);
    BMC->Mask=&(BMC->TG[n*L*2]);
    
    BMC->MuxID=malloc(sizeof(uint64_t)*n);
}

void BatchMuxGeneration(BatchMuxCore *BMC,uint64_t n,GateSet *Gate_Set);
void BatchMuxCheck(BMCRef *BMC_Ref,unsigned char a,unsigned char b,GateSet *Gate_Set);
#ifdef BOB
void BMCEval(BMCRef *BMC,W *Input1, W Input2, W* output,GateSet *Gate_Set);
#endif
#endif /* BatchMuxGeneration_h */
