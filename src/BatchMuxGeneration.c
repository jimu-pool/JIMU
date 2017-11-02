//
//  BatchMuxGeneration.c
//  
//
//  Created by Sourgroup on 9/13/17.
//
//

#include "BatchMuxGeneration.h"
void BatchMuxGeneration(BatchMuxCore *BMC,uint64_t n,GateSet *Gate_Set)
{
    if (n%16!=0) error("Batch Mux Generation Number error!\n");
    uint64_t L=BMC->L;
    WLRHash((BMC->BL),n*(2*L+1),&(Gate_Set->IHash));
    PBRHash((BMC->BLP),n*(2*L+1),&(Gate_Set->IHash));
 #ifdef ALICE
    block *tmpL0,*tmpR0,*tmpL1,*tmpR1,*tmpt;
    tmpL0=(block *)_mm_malloc(sizeof(block)*3*(n*L)*4,16);
    tmpR0=&(tmpL0[3*(n*L)]);
    tmpL1=&(tmpL0[6*(n*L)]);
    tmpR1=&(tmpL0[9*(n*L)]);
    tmpt=_mm_malloc(sizeof(block)*12*(n*L),16);
    
    block tmp,tmpDelta;
    WireLabel Delta;
    Delta=Gate_Set->Delta;
    tmpDelta=RMEnc(Delta,Gate_Set->RMTable);
    int i,j;
    
    for(i=0;i<n;i++)
    {
        BMC->MuxID[i]=Gate_Set->GateCount+i*L;
        tmp=RMEnc(BMC->BR[i],Gate_Set->RMTable);
        if (PBDec(BMC->BRP[i])==0)
        {
            tmpR0[3*i*L]=tmp;
            tmpR1[3*i*L]=_mm_xor_si128(tmp,tmpDelta);
        }
        else
        {
            tmpR1[3*i*L]=tmp;
            tmpR0[3*i*L]=_mm_xor_si128(tmp,tmpDelta);
        }
        for(j=L-1;j>=0;j--)
        {
            tmp=RMEnc(BMC->BL[i*L+j],Gate_Set->RMTable);
            if (PBDec(BMC->BLP[i*L+j])==0)
            {
                BMC->TE[i*L+j]=BMC->BL[i*L+j];
                tmpL0[3*(i*L+j)]=tmp;
                tmpL1[3*(i*L+j)]=_mm_xor_si128(tmp,tmpDelta);
            }
            else
            {
                BMC->TE[i*L+j]=WLabelXor(BMC->BL[i*L+j],Delta);
                tmpL1[3*(i*L+j)]=tmp;
                tmpL0[3*(i*L+j)]=_mm_xor_si128(tmp,tmpDelta);
            }
            tmp=_mm_set_epi32(0,0,0,6*(BMC->MuxID[i]+j)+1);
            tmpL0[3*(i*L+j)+1]=_mm_xor_si128(tmpL0[3*(i*L+j)],tmp);
            tmpL1[3*(i*L+j)+1]=_mm_xor_si128(tmpL1[3*(i*L+j)],tmp);
            
            tmp=_mm_set_epi32(0,0,0,6*(BMC->MuxID[i]+j)+2);
            tmpL0[3*(i*L+j)+2]=_mm_xor_si128(tmpL0[3*(i*L+j)],tmp);
            tmpL1[3*(i*L+j)+2]=_mm_xor_si128(tmpL1[3*(i*L+j)],tmp);
            
            
            tmp=_mm_set_epi32(0,0,0,6*(BMC->MuxID[i]+j));
            tmpL0[3*(i*L+j)]=_mm_xor_si128(tmpL0[3*(i*L+j)],tmp);
            tmpL1[3*(i*L+j)]=_mm_xor_si128(tmpL1[3*(i*L+j)],tmp);
            
            tmp=_mm_set_epi32(0,0,0,(6*(BMC->MuxID[i]+j)+4));
            tmpR0[3*(i*L+j)+1]=_mm_xor_si128(tmpR0[3*(i*L)],tmp);
            tmpR1[3*(i*L+j)+1]=_mm_xor_si128(tmpR1[3*(i*L)],tmp);
            
            
            tmp=_mm_set_epi32(0,0,0,(6*(BMC->MuxID[i]+j)+5));
            tmpR0[3*(i*L+j)+2]=_mm_xor_si128(tmpR0[3*(i*L)],tmp);
            tmpR1[3*(i*L+j)+2]=_mm_xor_si128(tmpR1[3*(i*L)],tmp);
            
            tmp=_mm_set_epi32(0,0,0,(6*(BMC->MuxID[i]+j)+3));
            tmpR0[3*(i*L+j)]=_mm_xor_si128(tmpR0[3*(i*L)],tmp);
            tmpR1[3*(i*L+j)]=_mm_xor_si128(tmpR1[3*(i*L)],tmp);
        }
    }
    Gate_Set->GateCount+=n*L;
    memcpy(tmpt,tmpL0,n*L*4*3*16);
    AES_ecb_encrypt_blks(tmpL0,12*n*L,&(Gate_Set->AesKey));
    for(i=0;i<n*12*L;i++)
        tmpL0[i]=_mm_xor_si128(tmpL0[i],tmpt[i]);
    
    int pl,pr,po;
    WireLabel HL0,HL1,HR0,HR1;
    
    for(i=0;i<n;i++)
    {
        pr=WLLSB(BMC->BR[i]);
        pr=pr^PBDec(BMC->BRP[i]);
        for(j=0;j<L;j++)
        {
            pl=WLLSB(BMC->BL[i*L+j]);
            pl=pl^PBDec(BMC->BLP[i*L+j]);
            po=PBDec(BMC->BOP[i*L+j]);
            HL0.L0=_mm256_set_m128i(tmpL0[3*(i*L+j)],tmpL0[3*(i*L+j)+1]);
            HL1.L0=_mm256_set_m128i(tmpL1[3*(i*L+j)],tmpL1[3*(i*L+j)+1]);
            HR0.L0=_mm256_set_m128i(tmpR0[3*(i*L+j)],tmpR0[3*(i*L+j)+1]);
            HR1.L0=_mm256_set_m128i(tmpR1[3*(i*L+j)],tmpR1[3*(i*L+j)+1]);
            HL0.L1=tmpL0[3*(i*L+j)+2];
            HL1.L1=tmpL1[3*(i*L+j)+2];
            HR0.L1=tmpR0[3*(i*L+j)+2];
            HR1.L1=tmpR1[3*(i*L+j)+2];
            BMC->TE[i*L+j]=WLabelXor(BMC->TE[i*L+j], WLabelXor(HR0,HR1));
            BMC->TG[i*L+j]= WLabelXor(HL0,HL1);
            if (pr==0) BMC->Mask[i*L+j]=HR0;
            else
            {
                BMC->Mask[i*L+j]= HR1;
                BMC->TG[i*L+j]=WLabelXor(BMC->TG[i*L+j],Delta);
            }
            BMC->Mask[i*L+j]=WLabelXor(BMC->Mask[i*L+j], HL0);
            if(pl==1) BMC->Mask[i*L+j]=WLabelXor(BMC->Mask[i*L+j],BMC->TG[i*L+j]);
            BMC->Mask[i*L+j]=WLabelXor(BMC->Mask[i*L+j],BMC->BO[i*L+j]);
            if (po==1)
                BMC->Mask[i*L+j]=WLabelXor(BMC->Mask[i*L+j],Delta);
        }
    }
    _mm_free(tmpL0);
    _mm_free(tmpt);
    psend((unsigned char *)(BMC->TG),sizeof(WireLabel)*n*L);
    psend((unsigned char *)(BMC->TE),sizeof(WireLabel)*n*L);
    psend((unsigned char *)(BMC->Mask),sizeof(WireLabel)*n*L);
#endif
#ifdef BOB
    int i;
    for(i=0;i<n;i++)
    {
        BMC->MuxID[i]=Gate_Set->GateCount+i*L;
    }
    preceive((unsigned char *)(BMC->TG),sizeof(WireLabel)*n*L);
    preceive((unsigned char *)(BMC->TE),sizeof(WireLabel)*n*L);
    preceive((unsigned char *)(BMC->Mask),sizeof(WireLabel)*n*L);
    Gate_Set->GateCount+=n*L;
#endif
}

#ifdef BOB
WireLabel BEHelper(uint64_t ID,WireLabel TG,WireLabel TE, WireLabel Mask, WireLabel l, WireLabel r,GateSet *Gate_Set)
{
    int sa,sb;
    sa=WLLSB(l);
    sb=WLLSB(r);
    WireLabel ga,gb;
    ga=Garbling(l,2*ID,Gate_Set);
    gb=Garbling(r,2*ID+1,Gate_Set);
    
    if(sa==1) ga=WLabelXor(ga,TG);
    if(sb==1) {
        gb=WLabelXor(gb,l);
        gb=WLabelXor(gb,TE);
    }
    ga=WLabelXor(ga,Mask);
    return WLabelXor(ga,gb);
}
void BMCEval(BMCRef *BMC,W *Input1, W Input2, W* output,GateSet *Gate_Set)
{
    int i;
    int L=BMC->BMC->L;
    for(i=0;i<L;i++)
    {
        output[i].L=BEHelper(BMC->BMC->MuxID[BMC->ID]+i,BMC->BMC->TG[BMC->ID*L+i],BMC->BMC->TE[BMC->ID*L+i],BMC->BMC->Mask[BMC->ID*L+i],Input1[i].L,Input2.L,Gate_Set);
    }
    return;
}
#endif

void BatchMuxCheck(BMCRef *BMC,unsigned char a,unsigned char b,GateSet *Gate_Set)
{
    int L=BMC->BMC->L;
    W *tmp;
    tmp=_mm_malloc(sizeof(W)*(2*L+1),32);
#ifdef ALICE
    WireLabel Delta;
    Delta=Gate_Set->Delta;
    W tmpW;
    int j;
    for(j=0;j<L;j++)
    {
        tmpW.P=BMC->BMC->BLP[j+L*BMC->ID];
        tmpW.L=BMC->BMC->BL[j+L*BMC->ID];
        if((PBDec(tmpW.P)^a)==1)
            tmpW.L=WLabelXor(tmpW.L,Delta);
        PsendPB(tmpW.P);
        PsendWL(tmpW.L);
    }
    tmpW.P=BMC->BMC->BRP[BMC->ID];
    tmpW.L=BMC->BMC->BR[BMC->ID];
    PsendPB(tmpW.P);
    if((PBDec(tmpW.P)^b)==1)
        tmpW.L=WLabelXor(tmpW.L,Delta);
    PsendWL(tmpW.L);
    for(j=0;j<L;j++)
    {
        tmpW.P=BMC->BMC->BOP[j+L*BMC->ID];
        PsendPB(tmpW.P);
    }
    return;
#endif
#ifdef BOB
    WLabelHash Delta;
    Delta=Gate_Set->Delta;
    PmtBit t;
    int j;
    for(j=0;j<L;j++)
    {
        PreceivePB(t);
        PreceiveWL(tmp[j].L);
        if(PBVerify(t,BMC->BMC->BLP[BMC->ID*L+j],&(Gate_Set->IHash))!=1)
            error("Check Error 1\n");
        if((PBDec(t)^a)==0)
        {
            if(WLVerify(tmp[j].L,BMC->BMC->BL[BMC->ID*L+j],&(Gate_Set->IHash))!=1)
            error("Check Error 2\n");
        }
        else
            if(WLVerify(tmp[j].L,WLHXor(BMC->BMC->BL[BMC->ID*L+j],Delta),&(Gate_Set->IHash))!=1) error("Check Error 3\n");
    }
    j=L;
    PreceivePB(t);
    PreceiveWL(tmp[j].L);
    if(PBVerify(t,BMC->BMC->BRP[BMC->ID],&(Gate_Set->IHash))!=1) error("Check Error 4\n");
    
    if((PBDec(t)^b)==0)
    {
        if(WLVerify(tmp[j].L,BMC->BMC->BR[BMC->ID],&(Gate_Set->IHash))!=1) error("Check Error 5\n");
    }
    else
        if(WLVerify(tmp[j].L,WLHXor(BMC->BMC->BR[BMC->ID],Delta),&(Gate_Set->IHash))!=1) error("Check Error 6\n");
    BMCEval(BMC,tmp,tmp[L],&(tmp[L+1]),Gate_Set);
    for(j=0;j<L;j++)
    {
        PreceivePB(t);
        if(PBVerify(t,BMC->BMC->BOP[BMC->ID*L+j],&(Gate_Set->IHash))!=1) error("Check Error 7\n");

        if((PBDec(t)^(a*b))==0)
        {
            if(WLVerify(tmp[L+1+j].L,BMC->BMC->BO[BMC->ID*L+j],&(Gate_Set->IHash))!=1) error("Check Error 8\n");
        }
        else
            if(WLVerify(tmp[L+1+j].L,WLHXor(BMC->BMC->BO[BMC->ID*L+j],Delta),&(Gate_Set->IHash))!=1) error("Check Error 9\n");
    }
    _mm_free(tmp);
#endif
}
