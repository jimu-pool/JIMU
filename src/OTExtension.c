//
//  OTExtension.c
//  
//
//  Created by Sourgroup on 7/18/17.
//
//

#include "OTExtension.h"
block * otRanGen(AES_KEY seed, uint64_t n,int n0)
{
    int i;
    block *tempseed;
    tempseed=(block *)malloc(sizeof(block)*n);
    if (tempseed==NULL) {printf("No enough memory!\n");return NULL;}
    for(i=0;i<n;i++)
    {
        tempseed[i]=_mm_set_epi32(0,0,0,(n0+i+32767));
    }
    AES_ecb_encrypt_blks(tempseed,n,&seed);
    
    for(i=0;i<n;i++)
    {
        tempseed[i]=_mm_xor_si128(tempseed[i],_mm_set_epi32(0,0,0,(n0+i)));
    }
    
    return tempseed;
}

//==========================base OT =============================
void baseotReceive(int b, block *a)
{
    block c[2];
    preceive((unsigned char *)&c[0],16);
    preceive((unsigned char *)&c[1],16);
    if(b==0) *a=c[0];
    else *a=c[1];
}
void baseotSend(block a, block b)
{
    psend((unsigned char *)&a,16);
    psend((unsigned char *)&b,16);
}
void otsetupsend(block delta, OTsetSend *ot)
{
    int i;
    for(i=0;i<128;i++)
    {
        if(i<32) bitpad[i]=_mm_set_epi32(0,0,0,(unsigned int)pow(2,i));
        else if (i<64) bitpad[i]=_mm_set_epi32(0,0,(unsigned int)pow(2,i-32),0);
        else if (i<96) bitpad[i]=_mm_set_epi32(0,(unsigned int)pow(2,i-64),0,0);
        else bitpad[i]=_mm_set_epi32((unsigned int)pow(2,i-96),0,0,0);
    }
    
    
    for(i=0;i<128;i++)
    {
        ot->otdeltac[127-i]=1^_mm_testz_si128(delta,bitpad[i]);
    }
    
    ot->otdelta=delta;
    
    for(i=0;i<128;i++)
    {
        baseotReceive(ot->otdeltac[i],&(ot->Seed[i]));
        AES_set_encrypt_key(ot->Seed[i], &(ot->extSeed[i]));
    }
    ot->otcount=0;
    ot->coeseed=_mm_set_epi32(1,2,3,4);
    
    psend((unsigned char *)&ot->coeseed,16);
    
    AES_set_encrypt_key(ot->coeseed, &(ot->extcoeseed));
}



void otsetupreceive(OTsetReceive *ot)
{
    int i,j;
    for(i=0;i<128;i++)
    {
        ot->Seed[i][0]=_mm_set_epi32(0,0,0,i);
        ot->Seed[i][1]=_mm_set_epi32(1,0,0,i);
    }
    
    for(i=0;i<128;i++)
    {
        baseotSend(ot->Seed[i][0],ot->Seed[i][1]);
        AES_set_encrypt_key(ot->Seed[i][0], &(ot->extSeed[i][0]));
        AES_set_encrypt_key(ot->Seed[i][1], &(ot->extSeed[i][1]));
    }

 //   flush();
    ot->randominputseed=_mm_set_epi32(123344,11122,3444,1144);
    preceive((unsigned char *)&ot->coeseed,16);
    AES_set_encrypt_key(ot->coeseed, &(ot->extcoeseed));
    AES_set_encrypt_key(ot->randominputseed, &(ot->extrandominputseed));
    
}

void otextsend(uint64_t n,block *out,OTsetSend *ot)
{
    block *t[128];
    uint64_t i;
    int j;
    for(i=0;i<128;i++)
    {
        t[i]=otRanGen(ot->extSeed[i],n+2,ot->otcount);
    }
    ot->otcount+=n+2;
    
    
    block *u[128];
    for(i=0;i<128;i++)
    {
        u[i]=malloc(sizeof(block)*(n+2));
        preceive((unsigned char *)(u[i]),16*(n+2));
        if(ot->otdeltac[i]==1)
            for(j=0;j<(n+2);j++)
                t[i][j]=_mm_xor_si128(u[i][j],t[i][j]);
    }
    
    
    block preas[8];
    unsigned char *tmpp[128],*tmpq[8];
    unsigned short *tmpr[8];
    for(i=0;i<8;i++)
    {
        tmpq[i]=(unsigned char *)&(preas[i]);
    }
    
    int k,l;
    for(i=0;i<n;i++)
    {
        for(j=0;j<128;j++)
            tmpp[j]=(unsigned char *)&(t[j][i]);
        for(j=15;j>=0;j--)
        {
            for(k=0;k<8;k++)
                for(l=0;l<16;l++)
                    tmpq[k][l]=tmpp[k*16+15-l][j];
            for(k=0;k<8;k++)
                tmpr[k]=(unsigned short *) &(out[i*128+(15-j)*8+k]);
            for(k=0;k<8;k++)
            {
                for(l=0;l<8;l++)
                {
                    tmpr[l][7-k]=_mm_movemask_epi8(preas[k]);
                    preas[k]=_mm_slli_epi64(preas[k],1);
                }
            }
            
        }
    }
    
    block tmpout[256];
    
    for(i=n;i<n+2;i++)
    {
        for(j=0;j<128;j++)
            tmpp[j]=(unsigned char *)&(t[j][i]);
        for(j=15;j>=0;j--)
        {
            for(k=0;k<8;k++)
                for(l=0;l<16;l++)
                    tmpq[k][l]=tmpp[k*16+15-l][j];
            for(k=0;k<8;k++)
                tmpr[k]=(unsigned short *) &(tmpout[(i-n)*128+(15-j)*8+k]);
            for(k=0;k<8;k++)
            {
                for(l=0;l<8;l++)
                {
                    tmpr[l][7-k]=_mm_movemask_epi8(preas[k]);
                    preas[k]=_mm_slli_epi64(preas[k],1);
                }
            }
            
        }
    }
    
    block *coe;
    coe=otRanGen(ot->extcoeseed,128*(n+2),ot->otcount*128);
    ot->otcount+=128*(n+2);
    
    block q[2],qtmp[2];
    q[0]=_mm_setzero_si128 ();
    q[1]=_mm_setzero_si128 ();
    for(i=0;i<n*128;i++)
    {
        mul128(out[i],coe[i],qtmp,&(qtmp[1]));
        q[0]=_mm_xor_si128(q[0],qtmp[0]);
        q[1]=_mm_xor_si128(q[0],qtmp[1]);
        
    }
    for(i=n*128;i<(n+2)*128;i++)
    {
        mul128(tmpout[i-128*n],coe[i],qtmp,&(qtmp[1]));
        q[0]=_mm_xor_si128(q[0],qtmp[0]);
        q[1]=_mm_xor_si128(q[0],qtmp[1]);
    }
    block rect[2],x;
    preceive((unsigned char *)rect,32);
    preceive((unsigned char *)&x,16);
    
    mul128(x,ot->otdelta,&(qtmp[0]),&(qtmp[1]));
    
    q[0]=_mm_xor_si128(q[0],rect[0]);
    q[1]=_mm_xor_si128(q[1],rect[1]);
    
    
   // if((_mm_testz_si128(q[0],q[0])!=1) ||(_mm_testz_si128(q[0],q[0])!=1)) printf("Correlation check fail");
    
    for(i=0;i<128;i++)
    {
        free(t[i]);
        free(u[i]);
    }
    free(coe);
}



void otextreceive(uint64_t n,block *in,block *out, OTsetReceive *ot)
{
    block *t[128][2];
    block *tmpin;
    
    tmpin=otRanGen(ot->extrandominputseed,2,ot->otcount);
    
    uint64_t i;
    int j;
    for(i=0;i<128;i++)
    {
        t[i][0]=otRanGen(ot->extSeed[i][0],n+2,ot->otcount);
        t[i][1]=otRanGen(ot->extSeed[i][1],n+2,ot->otcount);
    }
    ot->otcount+=n+2;
    
    
    
    block *u[128];
    for(i=0;i<128;i++)
    {
        u[i]=(block*)malloc(sizeof(block)*(n+2));
        if(u[i]==NULL) {printf("Not enough menory\n"); return;}
        for(j=0;j<(n);j++)
        {
            u[i][j]=_mm_xor_si128(t[i][0][j],t[i][1][j]);
            u[i][j]=_mm_xor_si128(u[i][j],in[j]);
        }
        for(j=n;j<(n+2);j++)
        {
            u[i][j]=_mm_xor_si128(t[i][0][j],t[i][1][j]);
            u[i][j]=_mm_xor_si128(u[i][j],tmpin[j-n]);
        }
        psend((unsigned char *)(u[i]),16*(n+2));
    }

    block preas[8];
    unsigned char *tmpp[128],*tmpq[8];
    unsigned short *tmpr[8];
    for(i=0;i<8;i++)
    {
        tmpq[i]=(unsigned char *)&(preas[i]);
    }
    
    int k,l;
    for(i=0;i<n;i++)
    {
        for(j=0;j<128;j++)
            tmpp[j]=(unsigned char *)&(t[j][0][i]);
        for(j=15;j>=0;j--)
        {
            for(k=0;k<8;k++)
                for(l=0;l<16;l++)
                    tmpq[k][l]=tmpp[k*16+15-l][j];
            for(k=0;k<8;k++)
                tmpr[k]=(unsigned short *) &(out[i*128+(15-j)*8+k]);
            for(k=0;k<8;k++)
            {
                for(l=0;l<8;l++)
                {
                    tmpr[l][7-k]=_mm_movemask_epi8(preas[k]);
                    preas[k]=_mm_slli_epi64(preas[k],1);
                }
            }
            
        }
    }
    block tmpout[256];
    
    for(i=n;i<n+2;i++)
    {
        for(j=0;j<128;j++)
            tmpp[j]=(unsigned char *)&(t[j][0][i]);
        for(j=15;j>=0;j--)
        {
            for(k=0;k<8;k++)
                for(l=0;l<16;l++)
                    tmpq[k][l]=tmpp[k*16+15-l][j];
            for(k=0;k<8;k++)
                tmpr[k]=(unsigned short *) &(tmpout[(i-n)*128+(15-j)*8+k]);
            for(k=0;k<8;k++)
            {
                for(l=0;l<8;l++)
                {
                    tmpr[l][7-k]=_mm_movemask_epi8(preas[k]);
                    preas[k]=_mm_slli_epi64(preas[k],1);
                }
            }
            
        }
    }
    
    
    block *coe;
    coe=otRanGen(ot->extcoeseed,128*(n+2),ot->otcount*128);
    ot->otcount+=128*(n+2);
    
    
    block st[2],tmpt[2];
    st[0]=_mm_setzero_si128();
    st[1]=_mm_setzero_si128();
    
    for(i=0;i<n*128;i++)
    {
        mul128(out[i],coe[i],tmpt,&(tmpt[1]));
        st[0]=_mm_xor_si128(st[0],tmpt[0]);
        st[1]=_mm_xor_si128(st[0],tmpt[1]);
        
    }
    for(i=n*128;i<(n+2)*128;i++)
    {
        mul128(tmpout[i-128*n],coe[i],tmpt,&(tmpt[1]));
        st[0]=_mm_xor_si128(st[0],tmpt[0]);
        st[1]=_mm_xor_si128(st[0],tmpt[1]);
    }
    
    block x;
    x=_mm_setzero_si128 ();
    
    for(i=0;i<n;i++)
    {
        tmpq[0]=(unsigned char *)&(in[i]);
        for(j=0;j<16;j++)
        {
            for(k=0;k<8;k++)
            {
                if((tmpq[0][15-j])>>(7-k) & 1)
                    x=_mm_xor_si128(x,coe[i*128+j*8+k]);
            }
        }
    }
    for(i=0;i<2;i++)
    {
        tmpq[0]=(unsigned char *)&(tmpin[i]);
        for(j=0;j<16;j++)
        {
            for(k=0;k<8;k++)
            {
                if((tmpq[0][15-j])>>(7-k) & 1)
                    x=_mm_xor_si128(x,coe[(n+i)*128+j*8+k]);
            }
        }
    }
    
    psend((unsigned char *)st,32);
    psend((unsigned char *)&x,16);
    
    
    for(i=0;i<128;i++)
    {
        free(t[i][0]);
        free(t[i][1]);
        free(u[i]);
    }
    free(coe);
    free(tmpin);
}



