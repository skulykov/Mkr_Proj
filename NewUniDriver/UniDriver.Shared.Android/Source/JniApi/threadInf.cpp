
#include "threadInf.h"
#include "my_time.h"
#include "Dbg.h"


int request4Monkey (int par1,float par2 ,cbThreadFunc_T _c)
{
      ReqThreadRequest_T arg;
    
    arg.par1=par1;
    
    arg.par2=par2;
    arg.cb=_c;
    
    //createRenderingThread
    {
        pthread_t thread = {0};
        if(pthread_create(&thread, NULL, threadRRProc, &arg) != 0)
        {
            fprintf(stderr,"ERROR threadProc... \n");//LOGE("ERROR threadProc... \n");

            return -1;
        }
    }
    
    return 1;
}
void  doRequest(int par1,char *payload)
{
    ReqThreadRequest_T *arg=new ReqThreadRequest_T();
    
    arg->par1=par1;
    arg->par2=2;
    arg->payload=payload;
    arg->cb=0;
    
    //createRenderingThread
    {
        pthread_t thread = {0};
        if(pthread_create(&thread, NULL, threadRRProc, (void *)arg) != 0)
        {
            fprintf(stderr,"ERROR threadProc... \n");//LOGE("ERROR threadProc... \n");

            if(arg!=0)
            {
                delete arg;
            }
			if (payload != 0)
				delete payload;//free(payload);
        }
    }
}
int doRequestRendering (int par1,char *payload  )
{
    
    fprintf(stderr,"doRequestRendering...Loop \n");
    int counter=0;
    int rv=0;
    while(1)
    {
        usleep(1*1000000);   // microseconds
       /*
		char *str= get_time();
        if(str!=NULL)
            m_printf("%s BBB :  doRequestRendering...Loop NN=%d\n",str,counter);
        else
        {
            uint64_t t1=getTOD();
			m_printf("%llu BBB :  doRequestRendering...Loop NN=%d\n",t1,counter);
        }
       // rv=m_connect("www.mikerel.com", 2411,payload);
		m_printf("%s BBB :  m_connect rv=%d\n",str,rv);
        if(rv==1)
        {
            break;
        }*/
        counter++;
    }
    
    return rv;
}

void *threadRRProc(void* _arg)
{
    ReqThreadRequest_T  *a=(ReqThreadRequest_T  *)_arg; ;
    if(a==0)
        return 0;
    
	m_printf("Start threadRRProc loop...%lx \n",(long)a->cb);
    
    int rv=doRequestRendering(a->par1,a->payload);
    
	m_printf("Finish threadRRProc...rv=%d \n",rv);
    if(a->cb!=0)
    {
		m_printf("Finish threadRRProc...Execute CB \n");

        a->cb();
    }
    if(a->payload!=0)
        free(a->payload);
    free(a);
    //sendSts(rv);
    return 0;
}
