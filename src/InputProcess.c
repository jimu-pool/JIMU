//
//  InputProcess.c
//  
//
//  Created by Sourgroup on 9/11/17.
//
//

#include "InputProcess.h"
#ifdef ALICE
unsigned char AliceOutput(Wire w,InputSet *Input_Set)
{
    
    WireLabel tmp;
    PreceiveWL(tmp);
    tmp=WLabelXor(tmp,w.L);
    if((_mm256_testz_si256(tmp.L0,tmp.L0)==1) && (_mm_testz_si128(tmp.L1,tmp.L1)==1)) return PBDec(w.P);
    tmp=WLabelXor(tmp,Input_Set->Gate_Set.Delta);
    if((_mm256_testz_si256(tmp.L0,tmp.L0)==1) && (_mm_testz_si128(tmp.L1,tmp.L1)==1)) return 1-PBDec(w.P);
    error("Alice Output Error\n");
    return 2;
}

unsigned char BobOutput(Wire w,InputSet *Input_Set){
    PsendPB(w.P);
    return 0;
}
void InputSetup(InputSet *Input_Set,block key)
{
    GateSetup(&(Input_Set->Gate_Set),key);
    otsetupsend(RMEnc(Input_Set->Gate_Set.Delta,Input_Set->Gate_Set.RMTable),&(Input_Set->OTSet));
    Input_Set->OTLabelPad[0]=_mm_set_epi32(0,0,0,1);
    Input_Set->OTLabelPad[1]=_mm_set_epi32(0,0,0,2);
    Input_Set->OTLabelPad[2]=RMEnc(Input_Set->Gate_Set.Delta,Input_Set->Gate_Set.RMTable);
    
    Input_Set->N=Alice_Input_Buffer_Size;
    Input_Set->index=0;
    PBRHash(Input_Set->P,Input_Set->N,&(Input_Set->Gate_Set.IHash));
    WLRHash(Input_Set->L,Input_Set->N,&(Input_Set->Gate_Set.IHash));
}

void AliceInputBufferRefresh(InputSet *Input_Set)
{
    Input_Set->index=0;
    PBRHash(Input_Set->P,Input_Set->N,&(Input_Set->Gate_Set.IHash));
    WLRHash(Input_Set->L,Input_Set->N,&(Input_Set->Gate_Set.IHash));
}

void AliceInput(Wire *dst,unsigned char *choice, uint64_t n,InputSet *Input_Set)
{
    uint64_t i;
    WireLabel tmp1;
    for(i=0;i<n;i++)
    {
        if(Input_Set->index>=Input_Set->N) AliceInputBufferRefresh(Input_Set);
        
        dst[i].P=Input_Set->P[Input_Set->index];
        dst[i].L=Input_Set->L[Input_Set->index];
        Input_Set->index++;
        if(choice[i]==PBDec(dst[i].P)) PsendWL(dst[i].L);
        else {
            tmp1=WLabelXor(dst[i].L,Input_Set->Gate_Set.Delta);
            PsendWL(tmp1);
        }
    }
}

void BobInput(Wire *dst, uint64_t n,InputSet *Input_Set)
{
    uint64_t n0=(n*40+127)/128;
    WireLabel *tmp,tmplabel[80];
    tmp=_mm_malloc(sizeof(WireLabel)*n0*128,32);
    WLRHash(tmp,n0*128,&(Input_Set->Gate_Set.IHash));
    block *out,*tmp1,*tmp2;
    out=_mm_malloc(sizeof(block)*n0*128,32);
    tmp1=_mm_malloc(sizeof(block)*n*240,32);
    tmp2=_mm_malloc(sizeof(block)*n*240,32);
    otextsend(n0,out,&(Input_Set->OTSet));
    uint64_t i,j;
    for(i=0;i<40*n;i++)
    {
        tmp1[i*6+0]=out[i];
        tmp1[i*6+1]=_mm_xor_si128(tmp1[i*6+0],Input_Set->OTLabelPad[0]);
        tmp1[i*6+2]=_mm_xor_si128(tmp1[i*6+0],Input_Set->OTLabelPad[1]);
        
        tmp1[i*6+3]=_mm_xor_si128(out[i],Input_Set->OTLabelPad[2]);
        tmp1[i*6+4]=_mm_xor_si128(tmp1[i*6+3],Input_Set->OTLabelPad[0]);
        tmp1[i*6+5]=_mm_xor_si128(tmp1[i*6+3],Input_Set->OTLabelPad[1]);
    }
    memcpy(tmp2,tmp1,16*n*240);
    AES_ecb_encrypt_blks(tmp1,n*240,&(Input_Set->Gate_Set.AesKey));
    for(i=0;i<n*240;i++)
        tmp1[i]=_mm_xor_si128(tmp1[i],tmp2[i]);
    
    
    for(i=0;i<n;i++)
    {
        for(j=0;j<40;j++)
        {
            tmplabel[2*j].L0=_mm256_set_m128i(tmp1[240*i+6*j],tmp1[240*i+6*j+1]);
            tmplabel[2*j].L1=tmp1[240*i+6*j+2];
            
            tmplabel[2*j+1].L0=_mm256_set_m128i(tmp1[240*i+6*j+3],tmp1[240*i+6*j+4]);
            tmplabel[2*j+1].L1=tmp1[240*i+6*j+5];
            
            tmplabel[2*j]=WLabelXor(tmplabel[2*j],tmp[40*i+j]);
            tmplabel[2*j+1]=WLabelXor(tmplabel[2*j+1],tmp[40*i+j]);
            tmplabel[2*j+1]=WLabelXor(tmplabel[2*j+1],Input_Set->Gate_Set.Delta);
        }
        psend((unsigned char*)tmplabel,80*sizeof(WireLabel));
        dst[i].P=_mm256_set_epi32(0,0,0,0,0,0,0,0);
        dst[i].L=tmp[40*i];
        for(j=1;j<40;j++)
            dst[i].L=WLabelXor(dst[i].L,tmp[40*i+j]);
        
    }
    _mm_free(tmp2);
    _mm_free(tmp1);
    _mm_free(out);
    _mm_free(tmp);
}
#endif
#ifdef BOB
unsigned char AliceOutput(WireE w,InputSet *Input_Set)
{
    PsendWL(w.L);
    return 0;
}
unsigned char BobOutput(WireE w,InputSet *Input_Set)
{
    PmtBit tmp;
    PreceivePB(tmp);
    if(PBVerify(tmp,w.W.P,&(Input_Set->Gate_Set.IHash))!=1) return 2;
    if(WLVerify(w.L,w.W.L,&(Input_Set->Gate_Set.IHash))==1) return PBDec(tmp);
    if(WLVerify(w.L,WLHXor(w.W.L,Input_Set->Gate_Set.Delta),&(Input_Set->Gate_Set.IHash))==1) return 1-PBDec(tmp);
    printf("\n\n");
    WLprint(w.L);
    WLprint(w.W.L.L);
    WLprint(WLHXor(w.W.L,Input_Set->Gate_Set.Delta).L);
    printf("\n\n");
    
    return 3;
}


void InputSetup(InputSet *Input_Set,block key)
{
    
    GateSetup(&(Input_Set->Gate_Set),key);
    otsetupreceive(&(Input_Set->OTSet));
    Input_Set->OTLabelPad[0]=_mm_set_epi32(0,0,0,1);
    Input_Set->OTLabelPad[1]=_mm_set_epi32(0,0,0,2);
    
    Input_Set->N=Alice_Input_Buffer_Size;
    Input_Set->index=0;
    PBRHash(Input_Set->P,Input_Set->N,&(Input_Set->Gate_Set.IHash));
    WLRHash(Input_Set->L,Input_Set->N,&(Input_Set->Gate_Set.IHash));

}

void AliceInputBufferRefresh(InputSet *Input_Set)
{
    Input_Set->index=0;
    PBRHash(Input_Set->P,Input_Set->N,&(Input_Set->Gate_Set.IHash));
    WLRHash(Input_Set->L,Input_Set->N,&(Input_Set->Gate_Set.IHash));
}

void AliceInput(WireE * dst, uint64_t n,InputSet *Input_Set)
{
    uint64_t i;
    for(i=0;i<n;i++)
    {
        if(Input_Set->index>=Input_Set->N) AliceInputBufferRefresh(Input_Set);
        dst[i].W.P=Input_Set->P[Input_Set->index];
        dst[i].W.L=Input_Set->L[Input_Set->index];
        Input_Set->index++;
        PreceiveWL(dst[i].L);
    }
    
}


void BobInput(WireE *dst,unsigned char *choice, uint64_t n,InputSet *Input_Set)
{
    uint64_t n0=(n*40+127)/128;
    WireLabel tmplabel[40],tmplabel1[80];
    WLabelHash *tmp;
    tmp=_mm_malloc(sizeof(WLabelHash)*n0*128,32);
    WLRHash(tmp,n0*128,&(Input_Set->Gate_Set.IHash));
    block *out,*choicem,*tmp2;
    out=_mm_malloc(sizeof(block)*n0*128,32);
    choicem=_mm_malloc(sizeof(block)*n0,32);
    unsigned char *tmpc;
    tmpc=malloc(sizeof(unsigned char)*n0*16);
    block *tmpout1,*tmpout2;
    tmpout1=_mm_malloc(sizeof(block)*n*120,32);
    tmpout2=_mm_malloc(sizeof(block)*n*120,32);
    
    
    uint64_t i,j;
    int k;
    unsigned char c;
    for(i=0;i<n;i++)
    {
        c=0;
        for(j=0;j<5;j++)
        {
            tmpc[i*5+j]=rand()%256;
            c=c^tmpc[i*5+j];
        }
        if(bytexorall(c)!=choice[i])tmpc[i*5+4]^=1;
    }
    for(i=n*5;i<n0*16;i++)
        tmpc[i]=0;
    for(i=0;i<n0;i++)
        
        choicem[i]=_mm_set_epi8(tmpc[i*16+0 ],tmpc[i*16+1],tmpc[i*16+2],tmpc[i*16+3],tmpc[i*16+4],tmpc[i*16+5],tmpc[i*16+6],tmpc[i*16+7],tmpc[i*16+8],tmpc[i*16+9],tmpc[i*16+10],tmpc[i*16+11],tmpc[i*16+12],tmpc[i*16+13],tmpc[i*16+14],tmpc[i*16+15]);
    otextreceive(n0,choicem,out,&(Input_Set->OTSet));
    
    for(i=0;i<n*40;i++)
    {
        tmpout1[3*i]=out[i];
        tmpout1[3*i+1]=_mm_xor_si128(out[i],Input_Set->OTLabelPad[0]);
        tmpout1[3*i+2]=_mm_xor_si128(out[i],Input_Set->OTLabelPad[1]);
    }
    memcpy(tmpout2,tmpout1,16*n*120);
    AES_ecb_encrypt_blks(tmpout1,n*120,&(Input_Set->Gate_Set.AesKey));
    for(i=0;i<n*120;i++)
        tmpout1[i]=_mm_xor_si128(tmpout1[i],tmpout2[i]);
    
    
    unsigned char tmpchoice;
    for(i=0;i<n;i++)
    {
        preceive((unsigned char*)tmplabel1,80*sizeof(WireLabel));
        dst[i].W.P.B=_mm256_set_epi32(0,0,0,0,0,0,0,0);
        dst[i].W.P.H=_mm256_set_epi32(0,0,0,0,0,0,0,0);
        
        for(j=0;j<5;j++)
        {
            tmpchoice=tmpc[i*5+j];
            for(k=7;k>=0;k--)
            {
                tmplabel[j*8+k].L0=_mm256_set_m128i(tmpout1[120*i+j*24+3*k],tmpout1[120*i+j*24+3*k+1]);
                tmplabel[j*8+k].L1=tmpout1[120*i+j*24+3*k+2];
                
                if(tmpchoice%2==0){
                    tmplabel[j*8+k]=WLabelXor(tmplabel[j*8+k],tmplabel1[16*j+k*2]);
                }
                else{
                    tmplabel[j*8+k]=WLabelXor(tmplabel[j*8+k],tmplabel1[16*j+k*2+1]);
                }
                tmpchoice=tmpchoice>>1;
                
                
            }
        }
        dst[i].W.L=tmp[i*40];
        dst[i].L=tmplabel[0];
        for(j=1;j<40;j++)
        {
            dst[i].W.L=WLHXor(tmp[i*40+j],dst[i].W.L);
            dst[i].L=WLabelXor(dst[i].L,tmplabel[j]);
        }
    }
    _mm_free(tmpout1);
    _mm_free(tmpout2);
    _mm_free(out);
    _mm_free(choicem);
    _mm_free(tmp);
    free(tmpc);
    
}
#endif
