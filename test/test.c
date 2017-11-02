//
//  test.c
//  
//
//  Created by Sourgroup on 9/7/17.
//
//


#include<stdio.h>
#include "InputProcess.h"
#include "BatchMuxBucketProcess.h"
int main(int argc, char ** argv)
{
    int c;
    c=atoi(argv[1]);
    opensocket(c);
    block key=_mm_set_epi32(1,2,3,4);
    InputSet Input_Set;
    InputSetup(&Input_Set,key);
#ifdef ALICE
    WireLabel testWL[256];
    PmtBit testPB[256];
#endif
#ifdef BOB
    WLabelHash testWL[256];
    PBitHash testPB[256];
#endif
    WLRHash(testWL,256,&(Input_Set.Gate_Set.IHash));
    PBRHash(testPB,256,&(Input_Set.Gate_Set.IHash));
    BatchMuxCore BMC;
    MallocBatchMuxCore(&BMC,32,128);
    BatchMuxGeneration(&BMC,128,&(Input_Set.Gate_Set));
    int i;
    BMCRef BMCr;
    BMCr.BMC=&BMC;
    
    for(i=0;i<128;i++)
    {
        BMCr.ID=i;
        BatchMuxCheck(&BMCr,0,0,&(Input_Set.Gate_Set));
        BatchMuxCheck(&BMCr,0,1,&(Input_Set.Gate_Set));
        BatchMuxCheck(&BMCr,1,0,&(Input_Set.Gate_Set));
        BatchMuxCheck(&BMCr,1,1,&(Input_Set.Gate_Set));
    }
    BMCBucket B;
    MallocBMCB(&B,32,128,1);
    for(i=0;i<128;i++)
    {
        B.BMC[i].BMC=&BMC;
        B.BMC[i].ID=i;
    }
    BMCIBSoldering(&B,128,&(Input_Set.Gate_Set));
    
    unsigned char bobchoice[32];
    for(i=0;i<32;i++)bobchoice[i]=1-i%2;
    
#ifdef ALICE
    Wire L,R;
    unsigned char x[2]={0,1};
    AliceInput(&L,&(x[0]),1,&Input_Set);
    AliceInput(&R,&(x[1]),1,&Input_Set);

    Wire testB[64];
    BobInput(testB,32,&Input_Set);
#endif
#ifdef BOB
    WireE L,R;
    AliceInput(&L,1,&Input_Set);
    AliceInput(&R,1,&Input_Set);
    
    WireE testB[64];
    BobInput(testB,bobchoice,32,&Input_Set);
#endif
    BMCBucketEval(&B,128,testB,R,&(testB[32]),&(Input_Set.Gate_Set));
    
    for(i=0;i<32;i++)
        printf("%d ",AliceOutput(testB[i],&Input_Set));
    printf("\n");
    for(i=32;i<64;i++)
        printf("%d ",AliceOutput(testB[i],&Input_Set));
    /*


    BatchGate BG;
    MallocBatchGate(&BG,25600);
    BatchGateGeneration(&BG,25600,&(Input_Set.Gate_Set));
    BGRef Ref;
    Ref.BG=&BG;
    Ref.ID=0;
    int i;
    for(i=0;i<2560;i++)
    {
        Ref.ID=i;
        BatchGateCheck(&Ref,0,0,&(Input_Set.Gate_Set));
        BatchGateCheck(&Ref,0,1,&(Input_Set.Gate_Set));
        BatchGateCheck(&Ref,1,0,&(Input_Set.Gate_Set));
        BatchGateCheck(&Ref,1,1,&(Input_Set.Gate_Set));

    }
    BatchBucket Bucket;
    MallocBatchBucket(&Bucket,2560);
    for(i=0;i<2560;i++)
    {
        Bucket.G[i].BG=&BG;
        Bucket.G[i].ID=i;
    }
    BatchBucketInnerSoldering(&Bucket,2560,&(Input_Set.Gate_Set));
    WireLabel Lin,Rin,dst;
#ifdef ALICE
    Wire L,R;
    unsigned char x[2]={0,1};
    AliceInput(&L,&(x[0]),1,&Input_Set);
    AliceInput(&R,&(x[1]),1,&Input_Set);
    
    BatchBucketOuterSoldering(L,R,&Bucket,&(Input_Set.Gate_Set));
    Lin=L.L;
    Rin=R.L;
    Wire testB[32];
#endif
#ifdef BOB
    WireE L,R;
    AliceInput(&L,1,&Input_Set);
    AliceInput(&R,1,&Input_Set);
    BatchBucketOuterSoldering(L.W,R.W,&Bucket,&(Input_Set.Gate_Set));
    Lin=L.L;
    Rin=R.L;
    WireE testB[32];
#endif
    
#ifdef BOB
BatchBucketLabelEval(&dst,&Bucket,Lin,Rin,2560,&(Input_Set.Gate_Set));
#endif
    printf("%d %d\n",AliceOutput(L,&Input_Set),AliceOutput(R,&Input_Set));
    
    unsigned char bobchoice[32];
    for(i=0;i<32;i++)bobchoice[i]=1-i%2;
#ifdef ALICE
    WLprint(testWL[0]);
    WLPprint(WLPad(testWL[0]));
    
    BobInput(testB,32,&Input_Set);
#endif
#ifdef BOB
    WLprint(testWL[0].L);
    WLPprint(WLPad(testWL[0].L));
    WLPprint(testWL[0].H);
    
    
    BobInput(testB,bobchoice,32,&Input_Set);
#endif
    for(i=0;i<32;i++)
        printf("%d ",AliceOutput(testB[i],&Input_Set));

    printf("\n\n");
    for(i=0;i<32;i++)
        printf("%d ",BobOutput(testB[i],&Input_Set));
    printf("\n");
     */
    
}
