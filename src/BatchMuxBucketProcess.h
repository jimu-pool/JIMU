//
//  BatchMuxBucketProcess.h
//  
//
//  Created by Sourgroup on 9/13/17.
//
//

#ifndef BatchMuxBucketProcess_h
#define BatchMuxBucketProcess_h

#include "BatchMuxGeneration.h"
typedef struct{
    uint64_t L;
    BMCRef *BMC;
#ifdef BOB
    WireLabel *ldiff,*rdiff,*odiff;
    WireLabel *lin,*rin;
#endif
}BMCBucket;
static inline void MallocBMCB(BMCBucket *BMCB,uint64_t L, uint64_t B,uint64_t n)
{
    uint64_t i;
    BMCB->BMC=(BMCRef*) malloc(sizeof(BMCRef)*B*n);
#ifdef BOB
    BMCB->ldiff=_mm_malloc(sizeof(WireLabel)*((2*L+1)*(B-1)+L+1)*n,32);
#endif
    
    for(i=0;i<n;i++)
    {
        BMCB[i].L=L;
        BMCB[i].BMC=&(BMCB[i].BMC[B*i]);
#ifdef BOB
        BMCB[i].ldiff=&(BMCB[0].ldiff[((2*L+1)*(B-1)+L+1)*i]);
        BMCB[i].rdiff=&(BMCB[i].ldiff[L*(B-1)]);
        BMCB[i].odiff=&(BMCB[i].ldiff[(L+1)*(B-1)]);
        BMCB[i].lin=&(BMCB[i].ldiff[(2*L+1)*(B-1)]);
        BMCB[i].rin=&(BMCB[i].ldiff[(2*L+1)*(B-1)+L]);
#endif
    }
    
}
void BMCIBSoldering(BMCBucket *BMCB,uint64_t BucketSize,GateSet *Gate_Set);
void BMCBucketEval(BMCBucket *c,uint64_t BucketSize, W *l, W r, W *Out,GateSet* Gate_Set);
#endif /* BatchMuxBucketProcess_h */
