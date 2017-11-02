
#ifndef util_h
#define util_h
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include  <fcntl.h>
#include <stdlib.h>
#include <string.h>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif
void error(const char *msg);
void opensocket(int portno);
long long inByte,outByte;
FILE *stream;
int sending;
inline uint64_t wallClock() {
    struct timespec t;
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    t.tv_sec = mts.tv_sec;
    t.tv_nsec = mts.tv_nsec;
    
#else
    clock_gettime(CLOCK_REALTIME, &t);
#endif
    return (t.tv_sec*1000*1000+t.tv_nsec/1000);
}

static inline void psend(unsigned char m[], int n)
{
    sending=1;
    fwrite(m,sizeof(unsigned char),n,stream);
    outByte+=n;
}

static inline void preceive(unsigned char m[], int n)
{
    if(sending==1)
    {
        sending=0;
        fflush(stream);
    }
    fread(m,sizeof(unsigned char),n,stream);
    inByte+=n;
}
static inline void networktest()
{
#ifdef ALICE
    unsigned char test[32];
    int jk;
    for(jk=0;jk<32;jk++)
        test[jk]=jk;
    psend(test,32);
#endif
#ifdef BOB
    printf("\n");
    unsigned char test[32];
    preceive(test,32);
    int jk;
    for(jk=0;jk<32;jk++)
        printf("%u ",test[jk]);
    printf("\n\n\n");
#endif
}

#endif
