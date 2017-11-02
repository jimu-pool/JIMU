//
//  RandomMatrix.h
//  
//
//  Created by Sourgroup on 9/13/16.
//
//

#ifndef RandomMatrix_h
#define RandomMatrix_h

#include <stdio.h>
#include "galois.h"
#include <gcrypt.h>
#include <emmintrin.h>
#include <immintrin.h>
#include "block.h"
block **RMTable;
block ** RMEncTable(int m,int n);
#endif /* RandomMatrix_h */
