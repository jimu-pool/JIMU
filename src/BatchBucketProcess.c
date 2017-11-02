//
//  BatchBucketProcess.c
//  
//
//  Created by Sourgroup on 9/11/17.
//
//

#include "BatchBucketProcess.h"
void BatchBucketInnerSoldering(BatchBucket *Bucket,uint64_t B,GateSet *Gate_Set)
{
#ifdef ALICE
    WireLabel Delta=Gate_Set->Delta;
    uint64_t i;
    PmtBit tmp;
    for(i=1;i<B;i++)
    {
        Bucket->DiffL[i-1]=WLabelXor(Bucket->G[i].BG->BL[Bucket->G[i].ID],Bucket->G[i-1].BG->BL[Bucket->G[i-1].ID]);
        tmp=PBitXor(Bucket->G[i].BG->BLP[Bucket->G[i].ID],Bucket->G[i-1].BG->BLP[Bucket->G[i-1].ID]);
        if (PBDec(tmp)==1) Bucket->DiffL[i-1]=WLabelXor(Bucket->DiffL[i-1],Delta);
        PsendPB(tmp);
        PsendWL(Bucket->DiffL[i-1]);
        
        
        Bucket->DiffR[i-1]=WLabelXor(Bucket->G[i].BG->BR[Bucket->G[i].ID],Bucket->G[i-1].BG->BR[Bucket->G[i-1].ID]);
        tmp=PBitXor(Bucket->G[i].BG->BRP[Bucket->G[i].ID],Bucket->G[i-1].BG->BRP[Bucket->G[i-1].ID]);
        if (PBDec(tmp)==1) Bucket->DiffR[i-1]=WLabelXor(Bucket->DiffR[i-1],Delta);
        PsendPB(tmp);
        PsendWL(Bucket->DiffR[i-1]);
        
        
        Bucket->DiffO[i-1]=WLabelXor(Bucket->G[i].BG->BO[Bucket->G[i].ID],Bucket->G[0].BG->BO[Bucket->G[0].ID]);
        tmp=PBitXor(Bucket->G[i].BG->BOP[Bucket->G[i].ID],Bucket->G[0].BG->BOP[Bucket->G[0].ID]);
        if (PBDec(tmp)==1) Bucket->DiffO[i-1]=WLabelXor(Bucket->DiffO[i-1],Delta);
        PsendPB(tmp);
        PsendWL(Bucket->DiffO[i-1]);
    }
#endif
#ifdef BOB
    WLabelHash Delta=Gate_Set->Delta;
    uint64_t i;
    PmtBit tmp;
    PBitHash tmph;
    WLabelHash tmpLh;
    for(i=1;i<B;i++)
    {
        PreceivePB(tmp);
        PreceiveWL(Bucket->DiffL[i-1]);
        tmph=PBHXor(Bucket->G[i].BG->BLPH[Bucket->G[i].ID],Bucket->G[i-1].BG->BLPH[Bucket->G[i-1].ID]);
        if(PBVerify(tmp,tmph,&(Gate_Set->IHash))!=1) error("Inner soldering error");
        tmpLh=WLHXor(Bucket->G[i].BG->BLH[Bucket->G[i].ID],Bucket->G[i-1].BG->BLH[Bucket->G[i-1].ID]);
        if(PBDec(tmp)==1) tmpLh=WLHXor(tmpLh,Delta);
        if(WLVerify(Bucket->DiffL[i-1],tmpLh,&(Gate_Set->IHash))!=1) error("Inner soldering error");
        
        PreceivePB(tmp);
        PreceiveWL(Bucket->DiffR[i-1]);
        tmph=PBHXor(Bucket->G[i].BG->BRPH[Bucket->G[i].ID],Bucket->G[i-1].BG->BRPH[Bucket->G[i-1].ID]);
        if(PBVerify(tmp,tmph,&(Gate_Set->IHash))!=1) error("Inner soldering error");
        tmpLh=WLHXor(Bucket->G[i].BG->BRH[Bucket->G[i].ID],Bucket->G[i-1].BG->BRH[Bucket->G[i-1].ID]);
        if(PBDec(tmp)==1) tmpLh=WLHXor(tmpLh,Delta);
        if(WLVerify(Bucket->DiffR[i-1],tmpLh,&(Gate_Set->IHash))!=1) error("Inner soldering error");
        
        PreceivePB(tmp);
        PreceiveWL(Bucket->DiffO[i-1]);
        tmph=PBHXor(Bucket->G[i].BG->BOPH[Bucket->G[i].ID],Bucket->G[0].BG->BOPH[Bucket->G[0].ID]);
        tmpLh=WLHXor(Bucket->G[i].BG->BOH[Bucket->G[i].ID],Bucket->G[0].BG->BOH[Bucket->G[0].ID]);
        if(PBVerify(tmp,tmph,&(Gate_Set->IHash))!=1) error("Inner soldering error");
        if(PBDec(tmp)==1) tmpLh=WLHXor(tmpLh,Delta);
        if(WLVerify(Bucket->DiffO[i-1],tmpLh,&(Gate_Set->IHash))!=1) error("Inner soldering error");
        
    }
#endif
}
void BatchBucketOuterSoldering (Wire L, Wire R, BatchBucket *C,GateSet *Gate_Set)
{
#ifdef ALICE
    WireLabel Delta=Gate_Set->Delta;
    PmtBit P1,P2;
    P1=PBitXor(L.P,C->G[0].BG->BLP[C->G[0].ID]);
    P2=PBitXor(R.P,C->G[0].BG->BRP[C->G[0].ID]);
    unsigned char a,b;
    a=PBDec(P1);
    b=PBDec(P2);
    
    C->Lin=WLabelXor(L.L,C->G[0].BG->BL[C->G[0].ID]);
    C->Rin=WLabelXor(R.L,C->G[0].BG->BR[C->G[0].ID]);
    if (a==1)
        C->Lin=WLabelXor(C->Lin,Delta);
    if (b==1)
        C->Rin=WLabelXor(C->Rin,Delta);
    PsendPB(P1);
    PsendPB(P2);
    PsendWL(C->Lin);
    PsendWL(C->Rin);
#endif
#ifdef BOB
    WLabelHash Delta=Gate_Set->Delta;
    PBitHash P1H,P2H;
    P1H=PBHXor(L.P,C->G[0].BG->BLPH[C->G[0].ID]);
    P2H=PBHXor(R.P,C->G[0].BG->BRPH[C->G[0].ID]);
    WLabelHash DiffLH,DiffRH;
    DiffLH=WLHXor(L.L,C->G[0].BG->BLH[C->G[0].ID]);
    DiffRH=WLHXor(R.L,C->G[0].BG->BRH[C->G[0].ID]);
    
    
    PmtBit P1,P2;
    PreceivePB(P1);
    PreceivePB(P2);
    
    PreceiveWL(C->Lin);
    PreceiveWL(C->Rin);
    
    unsigned char a,b;
    a=PBDec(P1);
    b=PBDec(P2);
    if (a==1)
        DiffLH=WLHXor(DiffLH,Delta);
    if (b==1)
        DiffRH=WLHXor(DiffRH,Delta);
    if (PBVerify(P1,P1H,&(Gate_Set->IHash))!=1) {
        PBprint(P1);
        PBprint(P1H.B);
        error("Outer soldering error");;
    }
    if (PBVerify(P2,P2H,&(Gate_Set->IHash))!=1) {
        PBprint(P2);
        PBprint(P2H.B);
        error("Outer soldering error");;
    }
    if (WLVerify(C->Lin,DiffLH,&(Gate_Set->IHash))!=1) {
        WLprint(C->Lin);
        WLprint(DiffLH.L);
        error("Outer soldering error");;
    }
    if (WLVerify(C->Rin,DiffRH,&(Gate_Set->IHash))!=1) {
        WLprint(C->Rin);
        WLprint(DiffRH.L);
        error("Outer soldering error");;
    }
#endif
}

#ifdef BOB
void BatchBucketLabelEval(WireLabel *dst, BatchBucket *Bucket, WireLabel Lin, WireLabel Rin,uint64_t B,GateSet *Gate_Set)
{
    WLabelHash Delta=Gate_Set->Delta;
    uint64_t i=0;
    Lin=WLabelXor(Lin,Bucket->Lin);
    Rin=WLabelXor(Rin,Bucket->Rin);
    WireLabel *Out;

    Out=(WireLabel *)_mm_malloc(sizeof(WireLabel)*B,32);
    Out[0]=BatchGateEval(&(Bucket->G[0]),Lin,Rin,Gate_Set);
    
    for(i=1;i<B;i++)
    {
        Lin=WLabelXor(Lin,Bucket->DiffL[i-1]);
        Rin=WLabelXor(Rin,Bucket->DiffR[i-1]);
        Out[i]=BatchGateEval(&(Bucket->G[i]),Lin,Rin,Gate_Set);
        Out[i]=WLabelXor(Out[i],Bucket->DiffO[i-1]);
    }
    
    uint64_t base=-1;
    unsigned char *signal;
    WireLabel tmp;
    signal=(unsigned char *)malloc(sizeof(unsigned char)*B);
    for(i=0;i<B;i++)
        signal[i]=(WLVerify(Out[i],Bucket->G[0].BG->BOH[Bucket->G[0].ID],&(Gate_Set->IHash))==1)||(WLVerify(Out[i],WLHXor(Bucket->G[0].BG->BOH[Bucket->G[0].ID],Delta),&(Gate_Set->IHash))==1);
    i=-1;
    do{
        i++;
    }while((signal[i]==0)&&(i<B));
    if (i==B)
    {
        printf("No good gate here!\n");
    }
    else
    {
        base=i;
        for(i=base+1;i<B;i++)
            if(signal[i]==1)
            {
                tmp=WLabelXor(Out[base],Out[i]);
                if ((_mm256_testz_si256(tmp.L0,tmp.L0)!=1) ||(_mm_testz_si128(tmp.L1,tmp.L1)!=1)) {error("Adversary input leakage!\n");}
            }
    }
    *dst=Out[0];
    free(signal);
    _mm_free(Out);
}
#endif

#ifdef ALICE
void BatchBucketEval(Wire *dst, BatchBucket *Bucket, Wire Lin, Wire Rin,uint64_t B,GateSet *Gate_Set)
{
    dst[0].P=Bucket->G[0].BG->BOP[Bucket->G[0].ID];
    dst[0].L=Bucket->G[0].BG->BO[Bucket->G[0].ID];
}
#endif
#ifdef BOB
void BatchBucketEval(WireE *dst, BatchBucket *Bucket, WireE Lin, WireE Rin,uint64_t B,GateSet *Gate_Set)
{
    dst[0].W.P=Bucket->G[0].BG->BOPH[Bucket->G[0].ID];
    dst[0].W.L=Bucket->G[0].BG->BOH[Bucket->G[0].ID];
    BatchBucketLabelEval(&(dst[0].L),Bucket,Lin.L,Rin.L,B,Gate_Set);
}

#endif

