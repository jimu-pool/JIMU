//
//  InputProcess.h
//  
//
//  Created by Sourgroup on 9/11/17.
//
//

#ifndef InputProcess_h
#define InputProcess_h
#include "BatchBucketProcess.h"
#include "OTExtension.h"
#define Alice_Input_Buffer_Size 1024

typedef struct{
    int N;
    int index;
#ifdef ALICE
    block OTLabelPad[3];
    PmtBit P[Alice_Input_Buffer_Size];
    WireLabel L[Alice_Input_Buffer_Size];
    OTsetSend OTSet;
#endif
#ifdef BOB
    PBitHash P[Alice_Input_Buffer_Size];
    WLabelHash L[Alice_Input_Buffer_Size];
    OTsetReceive OTSet;
    block OTLabelPad[2];
#endif
    
    GateSet Gate_Set;
}InputSet;
static inline unsigned char bytexorall(unsigned char b)
{
    int i;
    unsigned char b1=b;
    unsigned char c=0;
    for(i=0;i<8;i++)
    {
        c=c^b;
        b=b>>1;
    }
    return c%2;
}
#ifdef ALICE
unsigned char AliceOutput(Wire w,InputSet *Input_Set);
unsigned char BobOutput(Wire w,InputSet *Input_Set);
void AliceInput(Wire * __dst,unsigned char *choice, uint64_t n,InputSet *Input_Set);
void BobInput(Wire * __dst,uint64_t n,InputSet *Input_Set);
#endif
#ifdef BOB
unsigned char AliceOutput(WireE w,InputSet *Input_Set);
unsigned char BobOutput(WireE w,InputSet *Input_Set);
void AliceInput(WireE * __dst, uint64_t n,InputSet *Input_Set);
void BobInput(WireE * __dst,unsigned char *choice, uint64_t n,InputSet *Input_Set);
#endif
void InputSetup(InputSet *Input_Set,block key);
#endif /* InputProcess_h */
