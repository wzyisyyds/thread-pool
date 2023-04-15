#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdatomic.h>

#include "head.h"
#include "systemcallAPI.h"

typedef struct THREAD_POOL_DATA {

  UINT_64 Thread_max_size;

  UINT_64 task_max_size;

} Init_Thread_Pool_Data, *Ptr_Init_Thread_Pool_Data;

/*------------API---------------------*/
PVOID yexi_thread_pool_init(IN Init_Thread_Pool_Data data);

INT_64 yexi_thread_pool_push(IN PVOID ptr_thread_pool, IN PVOID arg, IN PVOID function);

INT_64 yexi_thread_pool_free(IN PVOID ptr_thread_pool);

#endif