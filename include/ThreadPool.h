#ifndef THREAD_POOL_H
#define THREAD_POOL_H



#include <stdatomic.h>



#include "head.h"
#include "systemcallAPI.h"



/*------------API---------------------*/
INT_64 yexi_thread_pool_init(OUT PVOID ptr_thread_pool);


INT_64 yexi_thread_pool_push(IN PVOID ptr_thread_pool, IN PVOID arg ,IN PVOID function);


#endif