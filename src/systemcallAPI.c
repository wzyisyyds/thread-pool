#include "../include/systemcallAPI.h"


/*local structs*/
typedef struct Mutex_Struct
{
    pthread_mutex_t mutex ;
    pthread_cond_t cond;

} Mutex,*Ptr_Mutex;




/*open API*/
PVOID yexi_mutex_create()
{
    Ptr_Mutex ptr= malloc(sizeof(Mutex));
    if (!pthread_mutex_init(&ptr->mutex,NULL)) {return NULL;}
    if (!pthread_cond_init(&ptr->cond,NULL)) {return NULL;}
    return ptr;
}


VOID yexi_mutex_free(PVOID prt_Mutex)
{
    free(prt_Mutex);
}

VOID yexi_mutex_lock(PVOID prt_Mutex)
{
    Ptr_Mutex ptr =prt_Mutex;
    pthread_mutex_lock(&ptr->mutex);
}

VOID yexi_mutex_unlock(PVOID prt_Mutex)
{
    Ptr_Mutex ptr =prt_Mutex;
    pthread_mutex_unlock(&ptr->mutex);
}

VOID yexi_cond_wait(PVOID prt_Mutex)
{
    Ptr_Mutex ptr =prt_Mutex;
    pthread_cond_wait(&ptr->cond,&ptr->mutex);
}

VOID yexi_cond_signal(PVOID prt_Mutex)
{
    Ptr_Mutex ptr =prt_Mutex;
    pthread_cond_signal(&ptr->cond);
}