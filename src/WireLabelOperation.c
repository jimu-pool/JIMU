//
//  WireLabelOperation.c
//  
//
//  Created by Sourgroup on 9/13/16.
//
//-lgcrypt -msse4.2 -march=corei7-avx -mavx2

#include "WireLabelOperation.h"

void print128(__m128i var)
{
    uint16_t *val = (uint16_t*) &var;
    printf("%04x %04x %04x %04x %04x %04x %04x %04x \n",
           val[7], val[6], val[5], val[4], val[3], val[2],
           val[1], val[0]);
}
void WLprint(WireLabel L)
{
    uint16_t *tmp;
    tmp=(uint16_t *)&(L.L0);
    int i;
    for(i=0;i<16;i++)
        printf("%04x ",tmp[i]);
    tmp=(uint16_t *)&(L.L1);
    for(i=0;i<8;i++)
        printf("%04x ",tmp[i]);

    //  for(i=0;i<16;i++)
   //     printf("%d ",(_mm_extract_epi8(L.b,i)+256)%256);
    printf("\n");
}
void WLPprint(WLabelPad L)
{
    int i;
    uint16_t *tmp;
    tmp=(uint16_t *)&(L.H0);
    for(i=0;i<16;i++)
        printf("%04x ",(tmp[i]));
    tmp=(uint16_t *)&(L.H1);
    for(i=0;i<8;i++)
        printf("%04x ",(tmp[i]));
    printf("\n");
}

WLabelPad RSWLRow(unsigned char *Mrow, unsigned char x, int m,int n, int w)
{
    WLabelPad TRow;
    unsigned char *tmp;
    tmp=(unsigned char *)malloc(sizeof(unsigned char)*(n-m));
    int i;
    for(i=0;i<n-m;i++)
    {
        tmp[i]=galois_multtable_multiply(Mrow[i+m],x,w);
      //  printf("%d ",tmp[i]);
    }
    TRow.H0=_mm256_set_epi8(tmp[31],tmp[30],tmp[29],tmp[28],tmp[27],tmp[26],tmp[25],tmp[24],tmp[23],tmp[22],tmp[21],tmp[20],tmp[19],tmp[18],tmp[17],tmp[16],tmp[15],tmp[14],tmp[13],tmp[12],tmp[11],tmp[10],tmp[9],tmp[8],tmp[7],tmp[6],tmp[5],tmp[4],tmp[3],tmp[2],tmp[1],tmp[0]);
    TRow.H1=_mm_set_epi8(0,0,0,0,0,0,0,0,tmp[39],tmp[38],tmp[37],tmp[36],tmp[35],tmp[34],tmp[33],tmp[32]);
    free(tmp);
    return TRow;
}
WLabelPad ** RSWLTable (int m,int n,int w)
{
    unsigned char **M;
    M=RSMatrix(m,n,w);
    int i,j;
    WLabelPad ** Table;
    Table=(WLabelPad **)_mm_malloc(sizeof(WLabelPad *)*m,32);
    for(i=0;i<m;i++)
    {
        Table[i]=(WLabelPad *)_mm_malloc(sizeof(WLabelPad)*powl(2,w),32);
       for(j=0;j<pow(2,w);j++)
            Table[i][j]=RSWLRow(M[i],(unsigned char)j,m,n,w);
    }
    for(i=0;i<m;i++)
        free(M[i]);
    free(M);
    return Table;
}
void freeRSWLTable()
{
    int i;
    for(i=0;i<WLL;i++)
        _mm_free(RSWLMatrix[i]);
    free(RSWLMatrix);
}


