//
//  IHash.h
//  
//
//  Created by Sourgroup on 9/10/17.
//
//

#ifndef IHash_h
#define IHash_h

#include <stdio.h>
#include "WireLabelOperation.h"
#include "util.h"
typedef struct{
    block WLSeed[WLN];
    block PBSeed[PBN];
    
    uint64_t IHashCount;
    AES_KEY PBSEEDKEY[PBN];
    AES_KEY WLSEEDKEY[WLN];
#ifdef BOB
    unsigned char WLwatch[WLN];
    unsigned char PBwatch[PBN];
    WLabelHash WLWatchPad;
    PBitHash PBWatchPad;
#endif
}IHashSet;
void IHashSetup();
#ifdef ALICE
WireLabel Flip(WireLabel a);
void WLRHash(WireLabel * dst, uint64_t n,IHashSet *IHash);
void PBRHash(PmtBit * dst, uint64_t n,IHashSet *IHash);
#endif

#ifdef BOB

WLabelHash Flip(WLabelHash a);
void WLRHash (WLabelHash * dst, uint64_t n,IHashSet *IHash);
void PBRHash(PBitHash * dst, uint64_t n,IHashSet *IHash);
unsigned char WLVerify(WireLabel L, WLabelHash H,IHashSet *IHash);
unsigned char PBVerify(PmtBit B, PBitHash H,IHashSet *IHash);
#endif

#endif /* IHash_h */
