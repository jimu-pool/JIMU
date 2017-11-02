//
//  RSEncoding.c
//  
//
//  Created by Sourgroup on 9/13/16.
//
//

#include "RSEncoding.h"
unsigned char ** RSMatrix(int m,int n,int w)
{
    galois_create_mult_tables(w);
    unsigned char ** M;
    int i;
    M=(unsigned char **)malloc(sizeof(unsigned char *)*m);
    for(i=0;i<m;i++)
    {
        M[i]=(unsigned char *)malloc(sizeof(unsigned char)*n);
    }
    for(i=0;i<n;i++)
        M[0][i]=i+1;
    for(i=1;i<m;i++)
    {
        int j;
        for(j=0;j<n;j++)
            M[i][j]=galois_multtable_multiply(M[i-1][j],M[0][j],w);
    }
    
    for (i=0;i<m;i++)
    {
        unsigned char factor;
        int j;
        for(j=i+1;j<m;j++)
        {
            factor=galois_multtable_divide(M[j][i],M[i][i],w);
            int k;
            for(k=i;k<n;k++)
                M[j][k]^=galois_multtable_multiply(factor,M[i][k],w);
        }
    }
    
    
    for(i=m-1;i>0;i--)
    {
        unsigned char factor;
        int j;
        for (j=i-1;j>=0;j--)
        {
            factor=galois_multtable_divide(M[j][i],M[i][i],w);
            int k;
            for(k=i;k<n;k++)
                M[j][k]^=galois_multtable_multiply(factor,M[i][k],w);
        }
    }
    for(i=0;i<m;i++)
    {
        int j;
        unsigned char factor=M[i][i];
        for(j=i;j<n;j++)
            M[i][j]=galois_multtable_divide(M[i][j],factor,w);
    }
    return M;
}
