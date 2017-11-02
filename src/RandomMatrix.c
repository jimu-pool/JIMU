//
//  RandomMatrix.c
//  
//
//  Created by Sourgroup on 9/13/16.
//
//

#include "RandomMatrix.h"
void RowSwitch (unsigned char **M, int x,int y,int n)
{
    int i;
    unsigned char t;
    for(i=0;i<n;i++)
    {
        t=M[x][i];
        M[x][i]=M[y][i];
        M[y][i]=t;
    }
}
int RowReduce (unsigned char **M,int x,int y, int m,int n)
{
    int i,j;
    for (i=x+1;i<m;i++)
    {
        unsigned char t;
        t=galois_multtable_divide(M[i][y],M[x][y],8);
        int sign=0;
        for (j=y;j<n;j++)
        {
            M[i][j]=M[i][j]^galois_multtable_multiply(t,M[x][j],8);
            if(M[i][j]!=0) sign=1;
        }
        if (sign==0) return 0;
    }
    return 1;
}
int LinearDependencyTest (unsigned char **M, int x,int y,int m,int n)
{
    if (x==m) return 1;
    if (y==n) return 0;
    int i,j;
    i=x-1;
    do{
        i++;
    }while((i<m)&&(M[i][y]==0));

    if (i==m) return LinearDependencyTest (M,x,y+1,m,n);
    if (i!=x) RowSwitch(M,x,i,n);
    if (RowReduce(M,x,y,m,n)==1) return LinearDependencyTest(M,x+1,y+1,m,n);
    else return 0;
}
unsigned char **GenRanMatrix(int m,int n)
{
    galois_create_mult_tables(8);
    unsigned char **M,**M1;
    int i;
    M=(unsigned char **)malloc(sizeof(unsigned char *)*m);
    M1=(unsigned char **)malloc(sizeof(unsigned char *)*m);
    for(i=0;i<m;i++)
    {
        M[i]=(unsigned char *)malloc(sizeof(unsigned char)*n);
        M1[i]=(unsigned char *)malloc(sizeof(unsigned char)*n);
        gcry_create_nonce(M[i],n);
        int j;
        for(j=0;j<n;j++)
        {
            //M[i][j]=rand()%256;
            M1[i][j]=M[i][j];
        }
    }
    do{
        for(i=0;i<m;i++)
        {
           gcry_create_nonce(M[i],n);
           int j;
           for(j=0;j<n;j++)
            {
          //      M[i][j]=rand()%256;
                M1[i][j]=M[i][j];
            }
        }
    }
    while (LinearDependencyTest(M1,0,0,m,n)==0);
    for(i=0;i<m;i++)
    free(M1[i]);
    free(M1);
    return M;
}
block ** RMEncTable(int m,int n)
{
    unsigned char **M;
    M=GenRanMatrix(m,n);
    block ** RMT;
    int i,j;
    RMT=(block **) _mm_malloc (sizeof (block*)*n,16);
    for (i=0;i<n;i++)
    RMT[i]=(block *)_mm_malloc(sizeof(block)*256,16);
    for(i=0;i<n;i++)
    {
        for(j=0;j<256;j++)
        {
            unsigned char temp[16];
            int k;
            for(k=0;k<m;k++)
            temp[k]=galois_multtable_multiply(j,M[k][i],8);
            RMT[i][j]=_mm_set_epi8(temp[15],temp[14],temp[13],temp[12],temp[11],temp[10],temp[9],temp[8],temp[7],temp[6],temp[5],temp[4],temp[3],temp[2],temp[1],temp[0]);
        }
    }
    for(i=0;i<m;i++)
        free(M[i]);
    free(M);
    return RMT;
}

