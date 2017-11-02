//
//  PermutationOperation.c
//  
//
//  Created by Sourgroup on 9/15/16.
//
//

#include "PermutationOperation.h"

void PBprint(PmtBit L)
{
    uint8_t *tmp;
    tmp=(uint8_t *)&(L);
    int i;
    for(i=0;i<32;i++)
        printf("%02x ",(tmp[i]+256)%256);
    printf("\n");
}
void PBPprint(PBitPad L)
{
    uint8_t *tmp;
    tmp=(uint8_t *)&(L);

    int i;
    for(i=0;i<32;i++)
        printf("%02x ",(tmp[i]+256)%256);
    printf("\n");
}
PBitPad PBRSRow(unsigned char *Mrow, unsigned char x, int m,int n, int w)
{
    PBitPad TRow;
    int i;
    TRow=_mm256_set_epi32(0,0,0,0,0,0,0,0);
    unsigned char *temp;
    temp=(unsigned char *)malloc(sizeof(unsigned char)*(n-m));
    for(i=0;i<n-m;i++)
        temp[i]=galois_multtable_multiply(Mrow[i+m],x,w);
    
    TRow=PBPConstruct(temp[0],temp[1],temp[2],temp[3],temp[4],temp[5],temp[6],temp[7],temp[8],temp[9],temp[10],temp[11],temp[12],temp[13],temp[14],temp[15],temp[16],temp[17],temp[18],temp[19],temp[20],temp[21],temp[22]);
    free(temp);
    return TRow;
}

PBitPad** RSPBTable (int m,int n,int w)
{
    unsigned char **M;
    M=RSMatrix(m,n,w);
    int i,j;
    PBitPad ** Table;
    Table=(PBitPad **)malloc(sizeof(PBitPad*)*m);
    for(i=0;i<m;i++)
    {
        Table[i]=(PBitPad *)_mm_malloc(sizeof(PBitPad)*powl(2,w),32);
        for(j=0;j<powl(2,w);j++)
            Table[i][j]=PBRSRow(M[i],(unsigned char)j,m,n,w);
    }
    for(i=0;i<m;i++)
        free(M[i]);
    free(M);
    return Table;
}

void freeRSPBTable()
{
    int i;
    for(i=0;i<PBL;i++)
        _mm_free(RSPBMatrix[i]);
    free(RSPBMatrix);
}

PBitPad PBPad (PmtBit b)
{
    PBitPad result;
    int i;
    uint8_t *tmp;
    tmp=(uint8_t *)&(b);

    result=RSPBMatrix[0][tmp[0]%64];
    for (i=1;i<21;i++)
    {
        result=PBPXor(result,RSPBMatrix[i][tmp[i]%64]);
    }
    return result;
}
/*
int PBDec(PmtBit B)
{
    int i;
    unsigned char tmp=0;
    uint8_t *t;
    t=(uint8_t *)&B;
    for(i=0;i<15;i++)
        tmp^=t[i];
    tmp^=t[15]%64;
    tmp=(tmp/16)^(tmp%16);
    tmp=(tmp/4)^(tmp%4);
    tmp=(tmp/2)^(tmp%2);
    return tmp;
}
*/
