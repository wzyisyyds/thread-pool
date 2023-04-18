#include "head.h"
#include <iostream>


extern"C" {    
    #include "../include/ThreadPool.h"

}


int main()
{
    Init_Thread_Pool_Data data;
    data.Thread_max_size=4;
    data.task_fast_size=16;
    data.task_slow_size=10;
    data.task_max_size=128;
    PVOID thread_pool=yexi_thread_pool_init(data);
    yexi_thread_pool_free(thread_pool);
    std::cout<<"hello word"<<std::endl;
    return 0;
}