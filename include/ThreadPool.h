#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "../include/head.h"
#include "../include/systemcallAPI.h"


typedef struct THREAD_TASK
{
    PVOID arg;
    VOID (*task_fun)(PVOID args);
    INT_8 task_ready;
} Task,*Ptr_Task;



typedef struct THREAD_POOL
{


} Pool,*Ptr_Pool;














#endif