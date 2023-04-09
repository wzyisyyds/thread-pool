


#include "../include/ThreadPool.h"



/*local structs*/

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

    atomic_flag push_Spin_lock;
    PVOID pop_mutex_lock;
    
    
    UINT_64 Thread_max_size;
    atomic_uint_fast64_t Thread_run_size;
    
    atomic_uint_fast64_t start;
    atomic_uint_fast64_t end;
    
    UINT_64 task_max_size;
    UINT_64 task_max_index;
    atomic_uint_fast64_t task_size;    

    Ptr_Task task_arry;
} Pool,*Ptr_Pool;




/*local function*/

static VOID Task_Thread();






VOID Task_Thread()
{

}



/*open API*/

INT_64 yexi_thread_pool_push(IN PVOID ptr_thread_pool, IN PVOID arg ,IN PVOID function)
{
    Ptr_Pool local_pool= ptr_thread_pool;
    
    if (local_pool->ThreadPool_mode._Value){
        //todo
    }
    else {
    
    
    while (atomic_flag_test_and_set(&local_pool->push_Spin_lock)) ;//Spin_lock

    if (local_pool->task_size >= local_pool->task_max_size) { 
        atomic_flag_clear(&local_pool->push_Spin_lock); 
        return YEXI_Statu_Unsuccess;    
    }


    UINT_64 end = atomic_fetch_add_explicit(&local_pool->end, 1, memory_order_acq_rel);
    if (end == (local_pool->task_max_index)) {
        atomic_exchange_explicit(&local_pool->end, 0, memory_order_acq_rel);
        atomic_flag_clear(&local_pool->push_Spin_lock); 
        return YEXI_Statu_Unsuccess;  
    }
    local_pool->task_arry[end].task_fun=function;
    local_pool->task_arry[end].arg = arg;
    atomic_fetch_add_explicit(&local_pool->task_size, 1, memory_order_acq_rel);
    atomic_flag_clear(&local_pool->push_Spin_lock);//unlock

    }
    return YEXI_Statu_Success;
}


