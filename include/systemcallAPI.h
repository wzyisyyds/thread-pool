#ifndef SYSTEMCALL_API_H
#define SYSTEMCALL_API_H

#include "stdlib.h"


#include <pthread.h>
#include <unistd.h>




#include "head.h"



/*------------API--------------*/

PVOID yexi_mutex_create();

INT_64 yexi_mutex_free(PVOID prt_Mutex);

INT_64 yexi_mutex_lock(PVOID prt_Mutex);

INT_64 yexi_mutex_unlock(PVOID prt_Mutex);

INT_64 yexi_cond_wait(PVOID prt_Mutex);

INT_64 yexi_cond_signal(PVOID prt_Mutex);

INT_64 yexi_cond_signal(PVOID prt_Mutex);

VOID yexi_thread_exit();

INT_32 yexi_pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine) (void *),
                   void *arg);


#endif