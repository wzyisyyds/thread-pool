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
    PVOID self;
    atomic_flag ThreadPool_mode;


    atomic_flag Push_Spin_lock;
    atomic_flag Pop_Spin_lock;

    
    UINT_64 Thread_max_size;
    atomic_uint_fast64_t Thread_run_size;
    
    atomic_uint_fast64_t start;
    atomic_uint_fast64_t end;
    
    UINT_64 task_max_size;
    atomic_uint_fast64_t task_size;    

    Ptr_Task task_arry;
} Pool,*Ptr_Pool;


INT_64 thread_pool_init_yexi(OUT PVOID ptr_thread_pool);





#endif