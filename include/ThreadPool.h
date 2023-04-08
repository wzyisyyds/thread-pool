#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "head.h"
#include "systemcallAPI.h"
#include <stdatomic.h>


typedef struct THREAD_TASK
{
    PVOID arg;
    VOID (*task_fun)(PVOID args);
    atomic_flag task_ready;
} Task,*Ptr_Task;



typedef struct THREAD_POOL
{
    
    atomic_uint_fast64_t start;
    atomic_uint_fast64_t end;
    UINT_64 task_size;    
    Ptr_Task task_arry;
} Pool,*Ptr_Pool;














#endif