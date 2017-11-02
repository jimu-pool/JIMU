//
//  BatchBucketProcess.h
//  
//
//  Created by Sourgroup on 9/11/17.
//
//

#ifndef BatchBucketProcess_h
#define BatchBucketProcess_h
#include "BatchGateGeneration.h"

#ifdef ALICE
static inline Wire XorGate(Wire Left, Wire Right)
{
    Wire XorGate_W;
    XorGate_W.P=PBitXor(Left.P,Right.P);
    XorGate_W.L=WLabelXor(Left.L,Right.L);
    return XorGate_W;
}
static inline Wire NotGate(Wire L,GateSet *Gate_Set)
{
    Wire NotGate_W;
    NotGate_W=L;
    NotGate_W.L=WLabelXor(NotGate_W.L,Gate_Set->Delta);
    return NotGate_W;
}
#endif
#ifdef BOB
static inline WireE XorGate(WireE Left, WireE Right)
{
    WireE XorGate_W;
    XorGate_W.L=WLabelXor(Left.L,Right.L);
    XorGate_W.W.P=PBHXor(Left.W.P,Right.W.P);
    XorGate_W.W.L=WLHXor(Left.W.L,Right.W.L);
    return XorGate_W;
}
static inline WireE NotGate(WireE L,GateSet *Gate_Set)
{
    L.W.L=WLHXor(L.W.L,Gate_Set->Delta);
    return L;
}
#endif
                           


typedef struct{
    BGRef *G;
    WireLabel *DiffL,*DiffR,*DiffO;
    WireLabel Lin,Rin;
}BatchBucket;


void BatchBucketInnerSoldering(BatchBucket *Bucket,uint64_t B,GateSet *Gate_Set);
void BatchBucketOuterSoldering (Wire L, Wire R, BatchBucket *C,GateSet *Gate_Set);
#ifdef BOB
void BatchBucketLabelEval(WireLabel *dst, BatchBucket *Bucket, WireLabel Lin, WireLabel Rin,uint64_t B,GateSet *Gate_Set);
#endif

static inline void MallocBatchBucket(BatchBucket *dst,uint64_t B,uint64_t n)
{
    dst[0].DiffL=_mm_malloc(sizeof(WireLabel)*(B-1)*3*n,32);
    dst[0].G=malloc(sizeof(BGRef)*B*n);
    uint64_t i;
    for(i=0;i<n;i++)
    {
        dst[i].DiffL=&(dst[0].DiffL[i*3*(B-1)]);
        dst[i].DiffR=&(dst[i].DiffL[B-1]);
        dst[i].DiffO=&(dst[i].DiffL[2*(B-1)]);
        dst[i].G=&(dst[0].G[B*i]);
    }
    return;
}
static inline void FreeBatchBucket(BatchBucket *B)
{
    _mm_free(B->DiffL);
    free(B->G);
}
#ifdef ALICE
void BatchBucketEval(Wire *dst, BatchBucket *Bucket, Wire Lin, Wire Rin,uint64_t B,GateSet *Gate_Set);
#endif
#ifdef BOB
void BatchBucketEval(WireE *dst, BatchBucket *Bucket, WireE Lin, WireE Rin,uint64_t B,GateSet *Gate_Set);
#endif
#endif /* BatchBucketProcess_h */
