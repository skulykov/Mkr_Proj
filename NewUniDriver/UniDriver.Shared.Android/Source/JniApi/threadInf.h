
#ifndef __THREADINF__interface__
#define __THREADINF__interface__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


typedef void (*cbThreadFunc_T)(void);

typedef struct  {
    
    int par1;
   	float par2;
    char *payload;
    cbThreadFunc_T cb;
}ReqThreadRequest_T;



    void *threadRRProc(void *arg);

    int request4Monkey(int par1,float par2 , cbThreadFunc_T _cb);
    void doRequest(int par1,char *payload);
    

void sendSts(int sts);

#endif /* defined(__VoIPHelper__interface__) */
