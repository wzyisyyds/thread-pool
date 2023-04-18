#include <iostream>


extern"C" {    
    #include "../include/ThreadPool.h"

}


int main()
{
    Init_Thread_Pool_Data data;
    data.Thread_max_size=1;
    data.task_fast_size=10;
    data.task_slow_size=5;
    data.task_max_size=100;
    yexi_thread_pool_free(yexi_thread_pool_init(data));
    std::cout<<"hello word"<<std::endl;
    return 0;
}