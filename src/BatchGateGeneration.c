//
//  BatchGateGeneration.c
//  
//
//  Created by Sourgroup on 9/10/17.
//
//

#include "BatchGateGeneration.h"
void GateSetup(GateSet *Gate_Set,block key)
{
    int i;
    IHashSetup(&(Gate_Set->IHash));
    AES_set_encrypt_key(key,&(Gate_Set->AesKey));
#ifdef ALICE
    Gate_Set->RMTable=RMEncTable(16,WLL);
    for(i=0;i<WLL;i++)
        psend((unsigned char *)(Gate_Set->RMTable[i]),256*16);
    WireLabel tmp[16];
    WLRHash(tmp,16,&(Gate_Set->IHash));
    
    unsigned char signal;
    signal=0;
    if (WLLSB(tmp[0])!=1)
    {
        signal=1;
        tmp[0]=Flip(tmp[0]);
    }
    psend(&signal,1);
    Gate_Set->Delta=tmp[0];
#endif
#ifdef BOB
    Gate_Set->RMTable=(block **)malloc(sizeof(block *)*WLL);
    for(i=0;i<WLL;i++)
    {
        Gate_Set->RMTable[i]=(block*)_mm_malloc(sizeof(block)*256,16);
        preceive((unsigned char *)(Gate_Set->RMTable[i]),256*16);
    }
    WLabelHash tmp[16];
    WLRHash(tmp,16,&(Gate_Set->IHash));
    unsigned char signal;
    preceive(&signal,1);
    if(signal==1)
        tmp[0]=Flip(tmp[0]);
    Gate_Set->Delta=tmp[0];
#endif
    Gate_Set->GateCount=0;
}
void BatchGateGeneration(BatchGate *dst, uint64_t n, GateSet *Gate_Set)
{
    if(n%16!=0) error("Generation Gate Number Wrong!");
#ifdef ALICE
    WireLabel Delta=Gate_Set->Delta;
    WLRHash((dst->BL),n*3,&(Gate_Set->IHash));
    PBRHash((dst->BLP),n*3,&(Gate_Set->IHash));
    block *tmpL0,*tmpR0,*tmpL1,*tmpR1,*tmpt;
    unsigned char pl,pr,po;
    tmpL0=(block *)_mm_malloc(sizeof(block)*n*3*4*2,16);
    tmpR0=&(tmpL0[3*n]);
    tmpL1=&(tmpL0[6*n]);
    tmpR1=&(tmpL0[9*n]);
    tmpt=&(tmpL0[12*n]);
    
    block tmp,tmpDelta;
    tmpDelta=RMEnc(Delta,Gate_Set->RMTable);
    
    
    uint64_t i;
    for(i=0;i<n;i++)
    {
        dst->GarbleID[i]=i+Gate_Set->GateCount;
        
        tmp=RMEnc(dst->BL[i],Gate_Set->RMTable);
        if (PBDec(dst->BLP[i])==0)
        {
            pl=1;
            dst->TE[i]=dst->BL[i];
            tmpL0[3*i]=tmp;
            tmpL1[3*i]=_mm_xor_si128(tmp,tmpDelta);
        }
        else
        {
            dst->TE[i]=WLabelXor(dst->BL[i],Delta);
            tmpL1[3*i]=tmp;
            tmpL0[3*i]=_mm_xor_si128(tmp,tmpDelta);
        }
        tmp=RMEnc(dst->BR[i],Gate_Set->RMTable);
        if (PBDec(dst->BRP[i])==0)
        {
            tmpR0[3*i]=tmp;
            tmpR1[3*i]=_mm_xor_si128(tmp,tmpDelta);
        }
        else
        {
            tmpR1[3*i]=tmp;
            tmpR0[3*i]=_mm_xor_si128(tmp,tmpDelta);
        }
        
        tmp=_mm_set_epi64x(0,6*(i+Gate_Set->GateCount)+1);
        tmpL0[3*i+1]=_mm_xor_si128(tmpL0[3*i],tmp);
        tmpL1[3*i+1]=_mm_xor_si128(tmpL1[3*i],tmp);
        
        tmp=_mm_set_epi64x(0,6*(i+Gate_Set->GateCount)+2);
        tmpL0[3*i+2]=_mm_xor_si128(tmpL0[3*i],tmp);
        tmpL1[3*i+2]=_mm_xor_si128(tmpL1[3*i],tmp);
        
        tmp=_mm_set_epi64x(0,6*(i+Gate_Set->GateCount));
        tmpL0[3*i]=_mm_xor_si128(tmpL0[3*i],tmp);
        tmpL1[3*i]=_mm_xor_si128(tmpL1[3*i],tmp);
        
        
        tmp=_mm_set_epi64x(0,6*(i+Gate_Set->GateCount)+5);
        tmpR0[3*i+2]=_mm_xor_si128(tmpR0[3*i],tmp);
        tmpR1[3*i+2]=_mm_xor_si128(tmpR1[3*i],tmp);
        
        tmp=_mm_set_epi64x(0,6*(i+Gate_Set->GateCount)+4);
        tmpR0[3*i+1]=_mm_xor_si128(tmpR0[3*i],tmp);
        tmpR1[3*i+1]=_mm_xor_si128(tmpR1[3*i],tmp);
        
        tmp=_mm_set_epi64x(0,6*(i+Gate_Set->GateCount)+3);
        tmpR0[3*i]=_mm_xor_si128(tmpR0[3*i],tmp);
        tmpR1[3*i]=_mm_xor_si128(tmpR1[3*i],tmp);
    }
    memcpy(tmpt,tmpL0,n*12*16);
    AES_ecb_encrypt_blks(tmpL0,12*n,&(Gate_Set->AesKey));
    for(i=0;i<n*12;i++)
        tmpL0[i]=_mm_xor_si128(tmpL0[i],tmpt[i]);
    for(i=0;i<n;i++)
    {
        
        pl=WLLSB(dst->BL[i]);
        pr=WLLSB(dst->BR[i]);
        po=PBDec(dst->BOP[i]);
        pl=pl^PBDec(dst->BLP[i]);
        pr=pr^PBDec(dst->BRP[i]);
        
        WireLabel HL0,HL1,HR0,HR1;
        HL0.L0=_mm256_set_m128i(tmpL0[3*i],tmpL0[3*i+1]);
        HL0.L1=tmpL0[3*i+2];
        
        HL1.L0=_mm256_set_m128i(tmpL1[3*i],tmpL1[3*i+1]);
        HL1.L1=tmpL1[3*i+2];
        
        HR0.L0=_mm256_set_m128i(tmpR0[3*i],tmpR0[3*i+1]);
        HR0.L1=tmpR0[3*i+2];
        
        HR1.L0=_mm256_set_m128i(tmpR1[3*i],tmpR1[3*i+1]);
        HR1.L1=tmpR1[3*i+2];
        
        
        dst->TE[i]=WLabelXor(dst->TE[i], WLabelXor(HR0,HR1));
        dst->TG[i]= WLabelXor(HL0,HL1);
        
        if (pr==0) dst->Mask[i]= HR0;
        else
        {
            dst->Mask[i]= HR1;
            dst->TG[i]=WLabelXor(dst->TG[i],Delta);
        }
        dst->Mask[i]=WLabelXor(dst->Mask[i], HL0);
        if(pl==1) dst->Mask[i]=WLabelXor(dst->Mask[i],dst->TG[i]);
        
        dst->Mask[i]=WLabelXor(dst->Mask[i],dst->BO[i]);
        if (po==1)
            dst->Mask[i]=WLabelXor(dst->Mask[i],Delta);
    }

    psend((unsigned char *)(dst->TG),sizeof(WireLabel)*n);
    psend((unsigned char *)(dst->TE),sizeof(WireLabel)*n);
    psend((unsigned char *)(dst->Mask),sizeof(WireLabel)*n);
    Gate_Set->GateCount+=n;
    _mm_free(tmpL0);
#endif
#ifdef BOB
    WLRHash((dst->BLH),n*3,&(Gate_Set->IHash));
    PBRHash((dst->BLPH),n*3,&(Gate_Set->IHash));
    uint64_t i;
    for(i=0;i<n;i++)
        dst->GarbleID[i]=i+Gate_Set->GateCount;
    
    preceive((unsigned char *)(dst->TG),sizeof(WireLabel)*n);
    preceive((unsigned char *)(dst->TE),sizeof(WireLabel)*n);
    preceive((unsigned char *)(dst->Mask),sizeof(WireLabel)*n);
    Gate_Set->GateCount+=n;
  #endif
}


WireLabel BatchGateEval(BGRef *G, WireLabel L1, WireLabel L2,GateSet *Gate_Set)
{
    unsigned char sa,sb;
    sa=WLLSB(L1);
    sb=WLLSB(L2);
    WireLabel ga,gb;
    ga=Garbling(L1,2*(G->BG->GarbleID[G->ID]),Gate_Set);
    gb=Garbling(L2,2*(G->BG->GarbleID[G->ID])+1,Gate_Set);
    
    if(sa==1) ga=WLabelXor(ga,G->BG->TG[G->ID]);
    if(sb==1) {
        gb=WLabelXor(gb,L1);
        gb=WLabelXor(gb,G->BG->TE[G->ID]);
    }
    ga=WLabelXor(ga,G->BG->Mask[G->ID]);
    return WLabelXor(ga,gb);
}


void BatchGateCheck (BGRef *G,unsigned char a,unsigned char b,GateSet *Gate_Set)
{
    #ifdef ALICE
    WireLabel Delta=Gate_Set->Delta;
    WireLabel L1,L2,Lout;
    if (a!=PBDec(G->BG->BLP[G->ID])) L1=WLabelXor(G->BG->BL[G->ID],Delta);
    else L1=G->BG->BL[G->ID];
    if (b!=PBDec(G->BG->BRP[G->ID])) L2=WLabelXor(G->BG->BR[G->ID],Delta);
    else L2=G->BG->BR[G->ID];
    PsendPB(G->BG->BLP[G->ID]);
    PsendPB(G->BG->BRP[G->ID]);
    PsendPB(G->BG->BOP[G->ID]);
    PsendWL(L1);
    PsendWL(L2);
    return;
#endif
#ifdef BOB
    
    
    WLabelHash Delta=Gate_Set->Delta;
    WireLabel L1,L2,Lout;
    PmtBit P1,P2,Pout;
    preceive((unsigned char*)&(P1),sizeof(PmtBit));
    preceive((unsigned char*)&(P2),sizeof(PmtBit));
    preceive((unsigned char*)&(Pout),sizeof(PmtBit));

    PreceiveWL(L1);
    PreceiveWL(L2);
    if ((PBVerify(P1,G->BG->BLPH[G->ID],&(Gate_Set->IHash))!=1)||(PBVerify(P2,G->BG->BRPH[G->ID],&(Gate_Set->IHash))!=1)||(PBVerify(Pout,G->BG->BOPH[G->ID],&(Gate_Set->IHash))!=1))
    {
        printf("error1\n");
        PBprint(P1);
        PBprint(P2);
        PBprint(Pout);
        
        
        printf("\n");
        PrintPB(G->BG->BLPH[G->ID]);
        PrintPB(G->BG->BRPH[G->ID]);
        PrintPB(G->BG->BOPH[G->ID]);
        exit(0);
    }
    unsigned char c=a*b;
    a=a^PBDec(P1);
    b=b^PBDec(P2);
    c=c^PBDec(Pout);
    if (a==0)
    {
        if (WLVerify(L1,G->BG->BLH[G->ID],&(Gate_Set->IHash))!=1)
        {
            printf("error2 %d\n",PBDec(P1));
            
            printf("Delta:\n");
            PrintWL(Gate_Set->Delta);
            printf("\nL1\n");

            WLprint(L1);
            WLPprint(WLPad(L1));
            printf("\nHL1\n");
            WLprint(G->BG->BLH[G->ID].L);
            WLPprint(G->BG->BLH[G->ID].H);
            WLPprint(WLPad(G->BG->BLH[G->ID].L));
            printf("\nHL1-\n");
            WLprint(WLHXor(G->BG->BLH[G->ID],Delta).L);
            WLPprint(WLPad(WLHXor(G->BG->BLH[G->ID],Delta).L));
            
            WLPprint(WLHXor(G->BG->BLH[G->ID],Delta).H);
            
            printf("\n\n");
            exit(0);
        }
    }
    else
        if (WLVerify(L1,WLHXor(G->BG->BLH[G->ID],Delta),&(Gate_Set->IHash))!=1) {
            printf("error3 %d\n",PBDec(P1));
            
            printf("Delta:\n");
            PrintWL(Gate_Set->Delta);
            printf("\n\n");

            WLprint(L1);
            WLPprint(WLPad(L1));
            printf("\n");
            WLprint(G->BG->BLH[G->ID].L);
            WLPprint(G->BG->BLH[G->ID].H);
            WLPprint(WLPad(G->BG->BLH[G->ID].L));
            printf("\n");
           WLprint(WLHXor(G->BG->BLH[G->ID],Delta).L);
           WLPprint(WLPad(WLHXor(G->BG->BLH[G->ID],Delta).L));
            
           WLPprint(WLHXor(G->BG->BLH[G->ID],Delta).H);
            
            printf("\n\n");
            exit(0);
            // printf("error3 %d\n",WLVerify(L1,WLHXor(G->BG->BLH[G->ID],Delta),&(Gate_Set->IHash)));
        }
    if (b==0)
    {
        if (WLVerify(L2,G->BG->BRH[G->ID],&(Gate_Set->IHash))!=1) {
            printf("error4\n");
            printf("Delta:\n");
            PrintWL(Gate_Set->Delta);
            printf("\n\n");
            
            WLprint(L2);
            WLPprint(WLPad(L2));
            printf("\n");
            WLprint(G->BG->BRH[G->ID].L);
            WLPprint(G->BG->BRH[G->ID].H);
            WLPprint(WLPad(G->BG->BRH[G->ID].L));
            printf("\n");
            WLprint(WLHXor(G->BG->BRH[G->ID],Delta).L);
            WLPprint(WLPad(WLHXor(G->BG->BRH[G->ID],Delta).L));
            
            WLPprint(WLHXor(G->BG->BRH[G->ID],Delta).H);
            
            printf("\n\n");
            exit(0);
        }
    }
    else
        if (WLVerify(L2,WLHXor(G->BG->BRH[G->ID],Delta),&(Gate_Set->IHash))!=1) {printf("error5\n");}
    Lout=BatchGateEval(G,L1,L2,Gate_Set);
    if (c==0)
    {
        if (WLVerify(Lout,G->BG->BOH[G->ID],&(Gate_Set->IHash))!=1) {printf("error6\n%d\n",WLVerify(Lout,WLHXor(G->BG->BOH[G->ID],Delta),&(Gate_Set->IHash)));
            WLprint(Lout);
            WLprint(G->BG->BOH[G->ID].L);
            WLprint(WLabelXor(G->BG->BOH[G->ID].L,(Gate_Set->Delta.L)));
            
            }
    }
    else
        if (WLVerify(Lout,WLHXor(G->BG->BOH[G->ID],Delta),&(Gate_Set->IHash))!=1) {printf("error7 %d\n",WLVerify(Lout,G->BG->BOH[G->ID],&(Gate_Set->IHash)));
            WLprint(Lout);
            WLprint(G->BG->BOH[G->ID].L);
            WLprint(WLabelXor(G->BG->BOH[G->ID].L,(Gate_Set->Delta.L)));

            }
#endif
}
