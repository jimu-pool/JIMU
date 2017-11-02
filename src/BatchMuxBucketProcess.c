//
//  BatchMuxBucketProcess.c
//  
//
//  Created by Sourgroup on 9/13/17.
//
//

#include "BatchMuxBucketProcess.h"
void BMCSoldering(BMCRef *c1,BMCRef *c2,WireLabel *difl,WireLabel *difr,WireLabel *difo,GateSet* Gate_Set)
{
#ifdef ALICE
    WireLabel Delta=Gate_Set->Delta;
    uint64_t L=c1->BMC->L;
    Wire tmp;
    int i;
    for(i=0;i<L;i++)
    {
        tmp.P=PBitXor(c1->BMC->BLP[c1->ID*L+i],c2->BMC->BLP[c2->ID*L+i]);
        tmp.L=WLabelXor(c1->BMC->BL[c1->ID*L+i],c2->BMC->BL[c2->ID*L+i]);
        if(PBDec(tmp.P)==1)
            tmp.L=WLabelXor(tmp.L,Delta);
      //  difl[i]=tmp.L;
        PsendPB(tmp.P);
        PsendWL(tmp.L);
    }
    {
        tmp.P=PBitXor(c1->BMC->BRP[c1->ID],c2->BMC->BRP[c2->ID]);
        tmp.L=WLabelXor(c1->BMC->BR[c1->ID],c2->BMC->BR[c2->ID]);
        if(PBDec(tmp.P)==1)
            tmp.L=WLabelXor(tmp.L,Delta);
     //   difr[0]=tmp.L;
        PsendPB(tmp.P);
        PsendWL(tmp.L);
    }
    for(i=0;i<L;i++)
    {
        tmp.P=PBitXor(c1->BMC->BOP[c1->ID*L+i],c2->BMC->BOP[c2->ID*L+i]);
        tmp.L=WLabelXor(c1->BMC->BO[c1->ID*L+i],c2->BMC->BO[c2->ID*L+i]);
        if(PBDec(tmp.P)==1)
            tmp.L=WLabelXor(tmp.L,Delta);
       // difo[i]=tmp.L;
        PsendPB(tmp.P);
        PsendWL(tmp.L);
    }
    return;
#endif
#ifdef BOB
    WLabelHash Delta=Gate_Set->Delta;
    PBitHash tmph;
    WLabelHash tmpl;
    PmtBit tmp;
    int L=c1->BMC->L;
    int i;
    for(i=0;i<L;i++)
    {
        PreceivePB(tmp);
        tmph=PBHXor(c1->BMC->BLP[c1->ID*L+i],c2->BMC->BLP[c2->ID*L+i]);
        if(PBVerify(tmp,tmph,&(Gate_Set->IHash))!=1){
            error("Mux inner soldering error 1");
        }
        tmpl=WLHXor(c1->BMC->BL[c1->ID*L+i],c2->BMC->BL[c2->ID*L+i]);
        if(PBDec(tmp)==1)
            tmpl=WLHXor(tmpl,Delta);
        PreceiveWL(difl[i]);
        if(WLVerify(difl[i],tmpl,&(Gate_Set->IHash))!=1) error("Mux inner soldering error 2");
    }
    
    {
        PreceivePB(tmp);
        tmph=PBHXor(c1->BMC->BRP[c1->ID],c2->BMC->BRP[c2->ID]);
        if(PBVerify(tmp,tmph,&(Gate_Set->IHash))!=1) error("Mux inner soldering error 3");
        tmpl=WLHXor(c1->BMC->BR[c1->ID],c2->BMC->BR[c2->ID]);
        if(PBDec(tmp)==1)
            tmpl=WLHXor(tmpl,Delta);
        PreceiveWL(difr[0]);
        if(WLVerify(difr[0],tmpl,&(Gate_Set->IHash))!=1) error("Mux inner soldering error 4");
    }
    
    for(i=0;i<L;i++)
    {
        PreceivePB(tmp);
        tmph=PBHXor(c1->BMC->BOP[c1->ID*L+i],c2->BMC->BOP[c2->ID*L+i]);
        if(PBVerify(tmp,tmph,&(Gate_Set->IHash))!=1) error("Mux inner soldering error 5");
        tmpl=WLHXor(c1->BMC->BO[c1->ID*L+i],c2->BMC->BO[c2->ID*L+i]);
        if(PBDec(tmp)==1)
            tmpl=WLHXor(tmpl,Delta);
        PreceiveWL(difo[i]);
        if(WLVerify(difo[i],tmpl,&(Gate_Set->IHash))!=1) error("Mux inner soldering error 6");
    }

#endif
}
void BMCIBSoldering(BMCBucket *BMCB,uint64_t BucketSize,GateSet *Gate_Set)
{
#ifdef ALICE
    int i;
    for(i=1;i<BucketSize;i++)
    {
        BMCSoldering(&(BMCB->BMC[0]),&(BMCB->BMC[i]),NULL,NULL,NULL,Gate_Set);
    }
#endif
#ifdef BOB
    int i;
    uint64_t L=BMCB->L;
    for(i=1;i<BucketSize;i++)
    {
        BMCSoldering(&(BMCB->BMC[0]),&(BMCB->BMC[i]),&(BMCB->ldiff[(i-1)*L]),&(BMCB->rdiff[i-1]),&(BMCB->odiff[(i-1)*L]),Gate_Set);
    }
#endif
}
void BMCOBSoldering(BMCBucket *B, W* l,W r,GateSet* Gate_Set)
{
#ifdef ALICE
    WireLabel Delta=Gate_Set->Delta;
    int i;
    BMCRef *c=B->BMC;
    int L=c->BMC->L;
    Wire tmp;
    for(i=0;i<L;i++)
    {
        tmp.P=PBitXor(c->BMC->BLP[c->ID*L+i],l[i].P);
        tmp.L=WLabelXor(c->BMC->BL[c->ID*L+i],l[i].L);
        
        if(PBDec(tmp.P)==1)
            tmp.L=WLabelXor(tmp.L,Delta);
     //   B->lin[i]=tmp.L;
        PsendPB(tmp.P);
        PsendWL(tmp.L);
    }
    
    tmp.P=PBitXor(c->BMC->BRP[c->ID],r.P);
    tmp.L=WLabelXor(c->BMC->BR[c->ID],r.L);
    if(PBDec(tmp.P)==1)
        tmp.L=WLabelXor(tmp.L,Delta);
   // B->rin=tmp.L;
    PsendPB(tmp.P);
    PsendWL(tmp.L);
#endif
#ifdef BOB
    WLabelHash Delta=Gate_Set->Delta;
    int i;
    BMCRef *c=B->BMC;
    PBitHash tmph;
    WLabelHash tmpl;
    PmtBit tmp;
    int L=c->BMC->L;
    for(i=0;i<L;i++)
    {
        PreceivePB(tmp);
        tmph=PBHXor(c->BMC->BLP[c->ID*L+i],l[i].W.P);
        
        if(PBVerify(tmp,tmph,&(Gate_Set->IHash))!=1)
            error("Mux Outer soldering error 1");
        tmpl=WLHXor(c->BMC->BL[c->ID*L+i],l[i].W.L);
        if(PBDec(tmp)==1)
            tmpl=WLHXor(tmpl,Delta);
        PreceiveWL(B->lin[i]);
        if(WLVerify(B->lin[i],tmpl,&(Gate_Set->IHash))!=1)
            error("Mux Outer soldering error 2");
    }
    
    {
        PreceivePB(tmp);
        tmph=PBHXor(c->BMC->BRP[c->ID],r.W.P);
        if(PBVerify(tmp,tmph,&(Gate_Set->IHash))!=1)
            error("Mux Outer soldering error 3");
        tmpl=WLHXor(c->BMC->BR[c->ID],r.W.L);
        if(PBDec(tmp)==1)
            tmpl=WLHXor(tmpl,Delta);
        PreceiveWL(*(B->rin));
        if(WLVerify(*(B->rin),tmpl,&(Gate_Set->IHash))!=1)
            error("Mux Outer soldering error 4");
    }
#endif
}
void BMCBucketEval(BMCBucket *c,uint64_t BucketSize, W *l, W r, W *Out,GateSet* Gate_Set)
{
#ifdef ALICE
    BMCOBSoldering(c,l,r,Gate_Set);
    uint64_t L=c->L;
    uint64_t i;
    for(i=0;i<L;i++)
    {
        Out[i].P=c->BMC[0].BMC->BOP[c->BMC[0].ID*L+i];
        Out[i].L=c->BMC[0].BMC->BO[c->BMC[0].ID*L+i];
    }
#endif
#ifdef BOB
    BMCOBSoldering(c,l,r,Gate_Set);
    uint64_t L=c->L;
    uint64_t B=BucketSize;
    W *in1,in2,*out;
    in1=_mm_malloc(sizeof(W)*L,32);
    
    int i,j;
    for(i=0;i<L;i++)
    {
        Out[i].W.P=c->BMC[0].BMC->BOP[c->BMC[0].ID*L+i];
        Out[i].W.L=c->BMC[0].BMC->BO[c->BMC[0].ID*L+i];
    }

    for(i=0;i<L;i++)
        in1[i]=l[i];
    in2=r;
    
    for(i=0;i<L;i++)
        in1[i].L=WLabelXor(in1[i].L,c->lin[i]);
    in2.L=WLabelXor(in2.L,(*(c->rin)));
    out=_mm_malloc(sizeof(W)*B*L,32);
    BMCEval(&(c->BMC[0]),in1,in2,&(out[0]),Gate_Set);
    for(i=1;i<B;i++)
    {
        for(j=0;j<L;j++)
            in1[j].L=WLabelXor(in1[j].L,c->ldiff[(i-1)*L+j]);
        in2.L=WLabelXor(in2.L,c->rdiff[i-1]);
        BMCEval(&(c->BMC[i]),in1,in2,&(out[i*L]),Gate_Set);
        for(j=0;j<L;j++)
            out[i*L+j].L=WLabelXor(out[i*L+j].L,c->odiff[(i-1)*L+j]);
    }
    _mm_free(in1);
    unsigned char sign;
    WLabelHash Delta=Gate_Set->Delta;
    for(i=0;i<L;i++)
    {
        sign=2;
        j=0;
        do
        {
            if(WLVerify(out[j*L+i].L,c->BMC[0].BMC->BO[c->BMC[0].ID*L+i],&(Gate_Set->IHash))==1)sign=0;
            if(WLVerify(out[j*L+i].L,WLHXor(c->BMC[0].BMC->BO[c->BMC[0].ID*L+i],Delta),&(Gate_Set->IHash))==1)sign=1;
            j++;
        }while((j<B)&&(sign==2));
        if(sign==2)error("No good Mux here\n");
        Out[i].L=out[(j-1)*L+i].L;
        for(;j<B;j++)
        {
            if(sign==0)
            {
                if(WLVerify(out[j*L+i].L,c->BMC[0].BMC->BO[c->BMC[0].ID*L+i],&(Gate_Set->IHash))!=1)
                {
                    if(WLVerify(out[j*L+i].L,WLHXor(c->BMC[0].BMC->BO[c->BMC[0].ID*L+i],Delta),&(Gate_Set->IHash))==1)
                        error("Adversary input leakage!\n");
                }
            }
            if(sign==1)
            {
                if(WLVerify(out[j*L+i].L,WLHXor(c->BMC[0].BMC->BO[c->BMC[0].ID*L+i],Delta),&(Gate_Set->IHash))!=1)
                {
                    if(WLVerify(out[j*L+i].L,c->BMC[0].BMC->BO[c->BMC[0].ID*L+i],&(Gate_Set->IHash))==1)
                        error("Adversary input leakage!\n");
                }
            }
        }
    }
    _mm_free(out);

#endif
}
