//
//  IHash.c
//  
//
//  Created by Sourgroup on 9/10/17.
//
//

#include "IHash.h"
#ifdef ALICE
void OT_w_out_n(int n,block *a)
{
    int i;
    for(i=0;i<n;i++)
        psend((unsigned char *)&(a[i]),sizeof(block));
}
block RanBlock()
{
    unsigned char tmp[16];
    gcry_create_nonce(tmp,16);
    return _mm_set_epi8(tmp[15],tmp[14],tmp[13],tmp[12],tmp[11],tmp[10],tmp[9],tmp[8],tmp[7],tmp[6],tmp[5],tmp[4],tmp[3],tmp[2],tmp[1],tmp[0]);
}
void IHashSetup(IHashSet *IHash)
{
    IHash->IHashCount=0;
    int i;
    for(i=0;i<WLN;i++)
        IHash->WLSeed[i]=RanBlock();
    for(i=0;i<PBN;i++)
        IHash->PBSeed[i]=RanBlock();
    OT_w_out_n(WLN,IHash->WLSeed);
    OT_w_out_n(PBN,IHash->PBSeed);
    for(i=0;i<WLN;i++)
        AES_set_encrypt_key(IHash->WLSeed[i],&(IHash->WLSEEDKEY[i]));
    
    for(i=0;i<PBN;i++)
        AES_set_encrypt_key(IHash->PBSeed[i],&(IHash->PBSEEDKEY[i]));
    
    
    RSWLMatrix=RSWLTable(WLL,WLN,8);
    RSPBMatrix=RSPBTable(PBL,PBN,6);
}

void WLRHash(WireLabel *result, uint64_t n,IHashSet *IHash)
{
    if(n%16!=0)error("IHash number wrong");
    unsigned char *temp[WLN],*temp0;
    uint64_t i,j;
    temp[0]=(unsigned char *)_mm_malloc(sizeof(block)*n*(WLN+1),16);
    for(i=1;i<WLN;i++)
    {
        temp[i]=&(temp[0][n*i]);
    }
    temp0=&(temp[0][n*WLN]);
    for(i=0;i<n;i+=16)
    {
         *((block*)&(temp[0][i]))=_mm_set_epi64x(IHash->IHashCount+i,IHash->IHashCount+i);
         *((block*)&(temp0[i]))=*((block*)&(temp[0][i]));
        for(j=1;j<WLN;j++)
        {
            *((block*)&(temp[j][i]))=*((block*)&(temp[0][i]));
        }
    }
    for(i=0;i<WLN;i++)
        AES_ecb_encrypt_blks((block*)(temp[i]),n/16,&(IHash->WLSEEDKEY[i]));
    for(i=0;i<WLN;i++)
        for(j=0;j<n;j+=16)
        {
            *((block*)&(temp[i][j]))=_mm_xor_si128(*((block*)&(temp[i][j])),*((block*)&(temp0[j])));
        }
    
    IHash->IHashCount+=n;
    for(i=0;i<n;i++)
        result[i]=WLConstruct(temp[0][i],temp[1][i],temp[2][i],temp[3][i],temp[4][i],temp[5][i],temp[6][i],temp[7][i],temp[8][i],temp[9][i],temp[10][i],temp[11][i],temp[12][i],temp[13][i],temp[14][i],temp[15][i],temp[16][i],temp[17][i],temp[18][i],temp[19][i],temp[20][i],temp[21][i],temp[22][i],temp[23][i],temp[24][i],temp[25][i],temp[26][i],temp[27][i],temp[28][i],temp[29][i],temp[30][i],temp[31][i],temp[32][i],temp[33][i],temp[34][i],temp[35][i],temp[36][i],temp[37][i],temp[38][i],temp[39][i],temp[40][i],temp[41][i],temp[42][i],temp[43][i],temp[44][i],temp[45][i],temp[46][i],temp[47][i]);
    
    WLabelPad h1,h2;
    
    for(i=0;i<n;i++)
    {
        h1=WLPad(result[i]);
        h2=WLPConstruct(temp[WLL][i],temp[WLL+1][i],temp[WLL+2][i],temp[WLL+3][i],temp[WLL+4][i],temp[WLL+5][i],temp[WLL+6][i],temp[WLL+7][i],temp[WLL+8][i],temp[WLL+9][i],temp[WLL+10][i],temp[WLL+11][i],temp[WLL+12][i],temp[WLL+13][i],temp[WLL+14][i],temp[WLL+15][i],temp[WLL+16][i],temp[WLL+17][i],temp[WLL+18][i],temp[WLL+19][i],temp[WLL+20][i],temp[WLL+21][i],temp[WLL+22][i],temp[WLL+23][i],temp[WLL+24][i],temp[WLL+25][i],temp[WLL+26][i],temp[WLL+27][i],temp[WLL+28][i],temp[WLL+29][i],temp[WLL+30][i],temp[WLL+31][i],temp[WLL+32][i],temp[WLL+33][i],temp[WLL+34][i],temp[WLL+35][i],temp[WLL+36][i],temp[WLL+37][i],temp[WLL+38][i],temp[WLL+39][i]);
        h1=WLPXor(h1,h2);
        PsendWLP(h1);
    }
    _mm_free(temp[0]);
    return;
}

void PBRHash(PmtBit * result,uint64_t n,IHashSet *IHash)
{
    if(n%16!=0)error("IHash number wrong");
    unsigned char *temp[PBN],*temp0;
    uint64_t i,j;
    temp[0]=(unsigned char *)_mm_malloc(sizeof(block)*n*(PBN+1),16);
    for(i=1;i<PBN;i++)
    {
        temp[i]=&(temp[0][n*i]);
    }
    temp0=&(temp[0][n*PBN]);
    for(i=0;i<n;i+=16)
    {
        *((block*)&(temp[0][i]))=_mm_set_epi64x(IHash->IHashCount+i,IHash->IHashCount+i);
        *((block*)&(temp0[i]))=*((block*)&(temp[0][i]));
        for(j=1;j<PBN;j++)
        {
            *((block*)&(temp[j][i]))=*((block*)&(temp[0][i]));
        }
    }
    for(i=0;i<PBN;i++)
        AES_ecb_encrypt_blks((block*)(temp[i]),n/16,&(IHash->PBSEEDKEY[i]));
    for(i=0;i<PBN;i++)
        for(j=0;j<n;j+=16)
        {
            *((block*)&(temp[i][j]))=_mm_xor_si128(*((block*)&(temp[i][j])),*((block*)&(temp0[j])));
        }
    
    IHash->IHashCount+=n;
    
    for(i=0;i<n;i++)
    {
        result[i]=PBConstruct(temp[0][i],temp[1][i],temp[2][i],temp[3][i],temp[4][i],temp[5][i],temp[6][i],temp[7][i],temp[8][i],temp[9][i],temp[10][i],temp[11][i],temp[12][i],temp[13][i],temp[14][i],temp[15][i],temp[16][i],temp[17][i],temp[18][i],temp[19][i],temp[20][i]);
    }
    PBitPad h1,h2;
    for(i=0;i<n;i++)
    {
        h2=PBPConstruct(temp[PBL][i],temp[PBL+1][i],temp[PBL+2][i],temp[PBL+3][i],temp[PBL+4][i],temp[PBL+5][i],temp[PBL+6][i],temp[PBL+7][i],temp[PBL+8][i],temp[PBL+9][i],temp[PBL+10][i],temp[PBL+11][i],temp[PBL+12][i],temp[PBL+13][i],temp[PBL+14][i],temp[PBL+15][i],temp[PBL+16][i],temp[PBL+17][i],temp[PBL+18][i],temp[PBL+19][i],temp[PBL+20][i],temp[PBL+21][i],temp[PBL+22][i]);
        //        PBPprint(h1);
        h1=PBPad(result[i]);
        h1=PBPXor(h1,h2);
        //psend((unsigned char *)&h1, 18);
        PsendPBP(h1);
    }
    _mm_free(temp[0]);
    return;
}
WireLabel Flip(WireLabel a)
{
    WireLabel tmp;
    tmp.L0=_mm256_set_epi32(0,0,0,0,0,0,0,0);
    tmp.L1=_mm_set_epi16(0,0,0,0,0,0,0,1);
    a=/*_mm256_xor_si256*/ WLabelXor(a,tmp);
    return a;
}
#endif
#ifdef BOB
void OT_w_out_n(int n, unsigned char *watch,block *a)
{
    int i;
    for(i=0;i<n;i++)
    {
        preceive((unsigned char *)&(a[i]),sizeof(block));
        if(watch[i]==0) a[i]=_mm_set_epi32(0,0,0,0);
    }
}
void IHashSetup(IHashSet *IHash)
{
    int i;
    for(i=0;i<WLN;i++)
        IHash->WLwatch[i]=255;
    for(i=0;i<PBN;i++)
        IHash->PBwatch[i]=63;
    OT_w_out_n(WLN,IHash->WLwatch,IHash->WLSeed);
    OT_w_out_n(PBN,IHash->PBwatch,IHash->PBSeed);
    for(i=0;i<WLN;i++)
        AES_set_encrypt_key(IHash->WLSeed[i],&(IHash->WLSEEDKEY[i]));
    
    for(i=0;i<PBN;i++)
        AES_set_encrypt_key(IHash->PBSeed[i],&(IHash->PBSEEDKEY[i]));
    
    RSWLMatrix=RSWLTable(WLL,WLN,8);
    RSPBMatrix=RSPBTable(21,44,6);
    
    IHash->WLWatchPad.L=WLConstruct(IHash->WLwatch[0],IHash->WLwatch[1],IHash->WLwatch[2],IHash->WLwatch[3],IHash->WLwatch[4],IHash->WLwatch[5],IHash->WLwatch[6],IHash->WLwatch[7],IHash->WLwatch[8],IHash->WLwatch[9],IHash->WLwatch[10],IHash->WLwatch[11],IHash->WLwatch[12],IHash->WLwatch[13],IHash->WLwatch[14],IHash->WLwatch[15],IHash->WLwatch[16],IHash->WLwatch[17],IHash->WLwatch[18],IHash->WLwatch[19],IHash->WLwatch[20],IHash->WLwatch[21],IHash->WLwatch[22],IHash->WLwatch[23],IHash->WLwatch[24],IHash->WLwatch[25],IHash->WLwatch[26],IHash->WLwatch[27],IHash->WLwatch[28],IHash->WLwatch[29],IHash->WLwatch[30],IHash->WLwatch[31],IHash->WLwatch[32],IHash->WLwatch[33],IHash->WLwatch[34],IHash->WLwatch[35],IHash->WLwatch[36],IHash->WLwatch[37],IHash->WLwatch[38],IHash->WLwatch[39],IHash->WLwatch[40],IHash->WLwatch[41],IHash->WLwatch[42],IHash->WLwatch[43],IHash->WLwatch[44],IHash->WLwatch[45],IHash->WLwatch[46],IHash->WLwatch[47]);
    
    IHash->WLWatchPad.H=WLPConstruct(IHash->WLwatch[WLL],IHash->WLwatch[WLL+1],IHash->WLwatch[WLL+2],IHash->WLwatch[WLL+3],IHash->WLwatch[WLL+4],IHash->WLwatch[WLL+5],IHash->WLwatch[WLL+6],IHash->WLwatch[WLL+7],IHash->WLwatch[WLL+8],IHash->WLwatch[WLL+9],IHash->WLwatch[WLL+10],IHash->WLwatch[WLL+11],IHash->WLwatch[WLL+12],IHash->WLwatch[WLL+13],IHash->WLwatch[WLL+14],IHash->WLwatch[WLL+15],IHash->WLwatch[WLL+16],IHash->WLwatch[WLL+17],IHash->WLwatch[WLL+18],IHash->WLwatch[WLL+19],IHash->WLwatch[WLL+20],IHash->WLwatch[WLL+21],IHash->WLwatch[WLL+22],IHash->WLwatch[WLL+23],IHash->WLwatch[WLL+24],IHash->WLwatch[WLL+25],IHash->WLwatch[WLL+26],IHash->WLwatch[WLL+27],IHash->WLwatch[WLL+28],IHash->WLwatch[WLL+29],IHash->WLwatch[WLL+30],IHash->WLwatch[WLL+31],IHash->WLwatch[WLL+32],IHash->WLwatch[WLL+33],IHash->WLwatch[WLL+34],IHash->WLwatch[WLL+35],IHash->WLwatch[WLL+36],IHash->WLwatch[WLL+37],IHash->WLwatch[WLL+38],IHash->WLwatch[WLL+39]);
    
    IHash->PBWatchPad.B=PBConstruct(IHash->PBwatch[0],IHash->PBwatch[1],IHash->PBwatch[2],IHash->PBwatch[3],IHash->PBwatch[4],IHash->PBwatch[5],IHash->PBwatch[6],IHash->PBwatch[7],IHash->PBwatch[8],IHash->PBwatch[9],IHash->PBwatch[10],IHash->PBwatch[11],IHash->PBwatch[12],IHash->PBwatch[13],IHash->PBwatch[14],IHash->PBwatch[15],IHash->PBwatch[16],IHash->PBwatch[17],IHash->PBwatch[18],IHash->PBwatch[19],IHash->PBwatch[20]);
    IHash->PBWatchPad.H=PBPConstruct(IHash->PBwatch[21],IHash->PBwatch[22],IHash->PBwatch[23],IHash->PBwatch[24],IHash->PBwatch[25],IHash->PBwatch[26],IHash->PBwatch[27],IHash->PBwatch[28],IHash->PBwatch[29],IHash->PBwatch[30],IHash->PBwatch[31],IHash->PBwatch[32],IHash->PBwatch[33],IHash->PBwatch[34],IHash->PBwatch[35],IHash->PBwatch[36],IHash->PBwatch[37],IHash->PBwatch[38],IHash->PBwatch[39],IHash->PBwatch[40],IHash->PBwatch[41],IHash->PBwatch[42],IHash->PBwatch[43]);
}


void WLRHash (WLabelHash *  result, uint64_t n,IHashSet *IHash)
{
    if(n%16!=0)error("IHash number wrong");
    unsigned char *temp[WLN],*temp0;
    uint64_t i,j;
    temp[0]=(unsigned char *)_mm_malloc(sizeof(block)*n*(WLN+1)/16,16);
    for(i=1;i<WLN;i++)
    {
        temp[i]=&(temp[0][n*i]);
    }
    temp0=&(temp[0][n*WLN]);
    for(i=0;i<n;i+=16)
    {
        *((block*)&(temp[0][i]))=_mm_set_epi64x(IHash->IHashCount+i,IHash->IHashCount+i);
        *((block*)&(temp0[i]))=*((block*)&(temp[0][i]));
        for(j=1;j<WLN;j++)
        {
            *((block*)&(temp[j][i]))=*((block*)&(temp[0][i]));
        }
    }
    for(i=0;i<WLN;i++)
        if(IHash->WLwatch[i]>0)
            AES_ecb_encrypt_blks((block*)(temp[i]),n/16,&(IHash->WLSEEDKEY[i]));
    for(i=0;i<WLN;i++)
        if(IHash->WLwatch[i]>0)
            for(j=0;j<n;j+=16)
            {
                
                *((block*)&(temp[i][j]))=_mm_xor_si128(*((block*)&(temp[i][j])),*((block*)&(temp0[j])));
            }
    
    IHash->IHashCount+=n;
    for(i=0;i<n;i++)
        result[i].L=WLConstruct(temp[0][i],temp[1][i],temp[2][i],temp[3][i],temp[4][i],temp[5][i],temp[6][i],temp[7][i],temp[8][i],temp[9][i],temp[10][i],temp[11][i],temp[12][i],temp[13][i],temp[14][i],temp[15][i],temp[16][i],temp[17][i],temp[18][i],temp[19][i],temp[20][i],temp[21][i],temp[22][i],temp[23][i],temp[24][i],temp[25][i],temp[26][i],temp[27][i],temp[28][i],temp[29][i],temp[30][i],temp[31][i],temp[32][i],temp[33][i],temp[34][i],temp[35][i],temp[36][i],temp[37][i],temp[38][i],temp[39][i],temp[40][i],temp[41][i],temp[42][i],temp[43][i],temp[44][i],temp[45][i],temp[46][i],temp[47][i]);
    
    WLabelPad h1,h2;
    for(i=0;i<n;i++)
    {
        
        h2=WLPConstruct(temp[WLL][i],temp[WLL+1][i],temp[WLL+2][i],temp[WLL+3][i],temp[WLL+4][i],temp[WLL+5][i],temp[WLL+6][i],temp[WLL+7][i],temp[WLL+8][i],temp[WLL+9][i],temp[WLL+10][i],temp[WLL+11][i],temp[WLL+12][i],temp[WLL+13][i],temp[WLL+14][i],temp[WLL+15][i],temp[WLL+16][i],temp[WLL+17][i],temp[WLL+18][i],temp[WLL+19][i],temp[WLL+20][i],temp[WLL+21][i],temp[WLL+22][i],temp[WLL+23][i],temp[WLL+24][i],temp[WLL+25][i],temp[WLL+26][i],temp[WLL+27][i],temp[WLL+28][i],temp[WLL+29][i],temp[WLL+30][i],temp[WLL+31][i],temp[WLL+32][i],temp[WLL+33][i],temp[WLL+34][i],temp[WLL+35][i],temp[WLL+36][i],temp[WLL+37][i],temp[WLL+38][i],temp[WLL+39][i]);
        PreceiveWLP(h1);
        result[i].H=WLPXor(h1,h2);
    }
    _mm_free(temp[0]);
    
    return;
}

void PBRHash(PBitHash * result, uint64_t n,IHashSet *IHash)
{
    if(n%16!=0)error("IHash number wrong");
    unsigned char *temp[PBN],*temp0;
    uint64_t i,j;
    temp[0]=(unsigned char *)_mm_malloc(sizeof(block)*n*(PBN+1),16);
    for(i=1;i<PBN;i++)
    {
        temp[i]=&(temp[0][n*i]);
    }
    temp0=&(temp[0][n*PBN]);
    for(i=0;i<n;i+=16)
    {
        *((block*)&(temp[0][i]))=_mm_set_epi64x(IHash->IHashCount+i,IHash->IHashCount+i);
        *((block*)&(temp0[i]))=*((block*)&(temp[0][i]));
        for(j=1;j<PBN;j++)
        {
            *((block*)&(temp[j][i]))=*((block*)&(temp[0][i]));
        }
    }
    for(i=0;i<PBN;i++)
        if(IHash->PBwatch[i]>0)
            AES_ecb_encrypt_blks((block*)(temp[i]),n/16,&(IHash->PBSEEDKEY[i]));
    for(i=0;i<PBN;i++)
        if(IHash->PBwatch[i]>0)
            for(j=0;j<n;j+=16)
            {
                 *((block*)&(temp[i][j]))=_mm_xor_si128(*((block*)&(temp[i][j])),*((block*)&(temp0[j])));
            }
    
    IHash->IHashCount+=n;
    
    for(i=0;i<n;i++)
    {
        result[i].B=PBConstruct(temp[0][i],temp[1][i],temp[2][i],temp[3][i],temp[4][i],temp[5][i],temp[6][i],temp[7][i],temp[8][i],temp[9][i],temp[10][i],temp[11][i],temp[12][i],temp[13][i],temp[14][i],temp[15][i],temp[16][i],temp[17][i],temp[18][i],temp[19][i],temp[20][i]);
    }
    
    PBitPad h1,h2;
    for(i=0;i<n;i++)
    {
        h2=PBPConstruct(temp[PBL][i],temp[PBL+1][i],temp[PBL+2][i],temp[PBL+3][i],temp[PBL+4][i],temp[PBL+5][i],temp[PBL+6][i],temp[PBL+7][i],temp[PBL+8][i],temp[PBL+9][i],temp[PBL+10][i],temp[PBL+11][i],temp[PBL+12][i],temp[PBL+13][i],temp[PBL+14][i],temp[PBL+15][i],temp[PBL+16][i],temp[PBL+17][i],temp[PBL+18][i],temp[PBL+19][i],temp[PBL+20][i],temp[PBL+21][i],temp[PBL+22][i]);
        PreceivePBP(h1);
        result[i].H=PBPXor(h1,h2);
    }
    
    
    _mm_free(temp[0]);
    
    return;
}
unsigned char WLVerify(WireLabel L, WLabelHash H,IHashSet *IHash)
{
    WLabelHash H1;
    H1.L=WLabelXor(L,H.L);
    H1.H=WLPad(L);
    H1.H=WLPXor(H1.H,H.H);
    
    if(_mm256_testz_si256(IHash->WLWatchPad.L.L0,H1.L.L0)==0) return -1;
    if(_mm_testz_si128(IHash->WLWatchPad.L.L1,H1.L.L1)==0) return -1;
    if(_mm256_testz_si256(IHash->WLWatchPad.H.H0,H1.H.H0)==0) return -3;
    if(_mm_testz_si128(IHash->WLWatchPad.H.H1,H1.H.H1)==0) return -4;
    
    return 1;
}
unsigned char PBVerify(PmtBit B, PBitHash H,IHashSet *IHash)
{
    PBitHash B1;
    B1.B=PBitXor(H.B,B);
    B1.H=PBPad(B);
    B1.H=PBPXor(B1.H,H.H);
    if (_mm256_testz_si256(IHash->PBWatchPad.B,B1.B)==0) return -1;
    if (_mm256_testz_si256(IHash->PBWatchPad.H,B1.H)==0) return -2;
    // if (((PBWatchPad.H.b[0]*B1.H.b[1])!=0) ||((PBWatchPad.H.b[0]*B1.H.b[1])!=0)) return -3;
    return 1;
}
WLabelHash Flip(WLabelHash a)
{
    WireLabel tmp;
    tmp.L0=_mm256_set_epi32(0,0,0,0,0,0,0,0);
    tmp.L1=_mm_set_epi16(0,0,0,0,0,0,0,1);
    a.L= WLabelXor(a.L,tmp);
    WLabelPad tmpp;
    tmpp=WLPad(tmp);
    a.H=WLPXor(a.H,tmpp);
    return a;
}
#endif
