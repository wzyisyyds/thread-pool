#include "../include/ThreadPool.h"

/*local structs*/

typedef struct THREAD_TASK {
    PVOID arg;
    VOID(*task_fun)
    (PVOID args);
    atomic_flag task_ready;
} Task, *Ptr_Task;

typedef struct THREAD_POOL {
    PVOID self;

    pthread_t pthread_id[256];

    atomic_flag ThreadPool_mode;

    atomic_flag ThreadPool_KILL;

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
} Pool, *Ptr_Pool;

/*local function*/

static INT_64 yexi_thread_pool_pop(IN PVOID ptr_thread_pool) {
    Ptr_Pool local_pool = ptr_thread_pool;

    PVOID arg;
    VOID(*function)
    (PVOID);

Restart: // loop

    if (local_pool->ThreadPool_mode._Value) {
        UINT_64 start = atomic_fetch_add_explicit(&local_pool->start, 1, memory_order_acq_rel);
        if (start < (local_pool->task_max_index)) {
            while (!local_pool->task_arry[start].task_ready._Value)
                ;
            atomic_fetch_sub_explicit(&local_pool->task_size, 1, memory_order_acq_rel);
            function = local_pool->task_arry[start].task_fun;
            arg = local_pool->task_arry[start].arg;
            atomic_flag_clear(&local_pool->task_arry[start].task_ready);

        } else if (start == (local_pool->task_max_index)) {
            atomic_exchange_explicit(&local_pool->start, 0, memory_order_acq_rel);
        } else {
            goto Restart;
        }
    } else {
        yexi_mutex_lock(local_pool->pop_mutex_lock);

        if (local_pool->task_size <= 0) {
            yexi_mutex_unlock(local_pool->pop_mutex_lock);
            atomic_fetch_sub_explicit(&local_pool->Thread_run_size, 1, memory_order_acq_rel);
            yexi_cond_wait(local_pool->pop_mutex_lock);
            yexi_mutex_unlock(local_pool->pop_mutex_lock);
            atomic_fetch_add_explicit(&local_pool->Thread_run_size, 1, memory_order_acq_rel);
            return YEXI_Statu_Success;
        }

        UINT_64 start = atomic_fetch_add_explicit(&local_pool->start, 1, memory_order_acq_rel);
        if (start < (local_pool->task_max_index)) {
            while (!local_pool->task_arry[start].task_ready._Value)
                ;
            atomic_fetch_sub_explicit(&local_pool->task_size, 1, memory_order_acq_rel);
            function = local_pool->task_arry[start].task_fun;
            arg = local_pool->task_arry[start].arg;
            atomic_flag_clear(&local_pool->task_arry[start].task_ready);
        } else if (start == (local_pool->task_max_index)) {
            atomic_exchange_explicit(&local_pool->start, 0, memory_order_acq_rel);
            yexi_mutex_unlock(local_pool->pop_mutex_lock);
            return YEXI_Statu_Unsuccess;
        } else {
            yexi_mutex_unlock(local_pool->pop_mutex_lock);
            return YEXI_Statu_Unsuccess;
        }
        yexi_mutex_unlock(local_pool->pop_mutex_lock);
    }

    function(arg);
    return YEXI_Statu_Success;
}

static PVOID yexi_thread_task(IN PVOID ptr_thread_pool) {
    Ptr_Pool local_pool = ptr_thread_pool;
    atomic_fetch_add_explicit(&local_pool->Thread_run_size, 1, memory_order_acq_rel);
    while (1) {
        yexi_thread_pool_pop(local_pool);

        if (local_pool->ThreadPool_KILL._Value) {
            atomic_fetch_sub_explicit(&local_pool->Thread_run_size, 1, memory_order_acq_rel);
            yexi_thread_exit();
        }
    }
    return YEXI_Statu_Success;
}

/*open API*/

PVOID yexi_thread_pool_init(IN Init_Thread_Pool_Data data) {
    Ptr_Pool pool = malloc(sizeof(Pool));
    pool->task_max_size = data.task_max_size;
    pool->task_max_index = pool->task_max_size + 1023;
    pool->Thread_max_size = data.task_max_size;
    pool->self = pool;

    pool->pop_mutex_lock = yexi_mutex_create();

    pool->task_arry = malloc(sizeof(Task) * (pool->task_max_index + 1));

    for (int i = 0; i < pool->Thread_max_size; i++) {
        yexi_pthread_create_and_detach(yexi_thread_task, pool->self);
    }
    return pool->self;
}

INT_64 yexi_thread_pool_push(IN PVOID ptr_thread_pool, IN PVOID arg, IN PVOID function) {
    Ptr_Pool local_pool = ptr_thread_pool;

Restart: // loop

    if (local_pool->ThreadPool_mode._Value) {
        if (local_pool->task_size < local_pool->task_max_size) {
            UINT_64 end = atomic_fetch_add_explicit(&local_pool->end, 1, memory_order_acq_rel);
            if (end < (local_pool->task_max_index)) {
                local_pool->task_arry[end].task_fun = function;
                local_pool->task_arry[end].arg = arg;
                atomic_flag_test_and_set(&local_pool->task_arry[end].task_ready);
                atomic_fetch_add_explicit(&local_pool->task_size, 1, memory_order_acq_rel);
            } else if (end == (local_pool->task_max_index)) {
                atomic_exchange_explicit(&local_pool->end, 0, memory_order_acq_rel);
                goto Restart;
            } else {
                goto Restart;
            }

        } else {
            return YEXI_Statu_Unsuccess;
        }
    } else {
        while (atomic_flag_test_and_set(&local_pool->push_Spin_lock))
            ; // Spin_lock

        if (local_pool->task_size < local_pool->task_max_size) {
            UINT_64 end = atomic_fetch_add_explicit(&local_pool->end, 1, memory_order_acq_rel);
            if (end < (local_pool->task_max_index)) {
                local_pool->task_arry[end].task_fun = function;
                local_pool->task_arry[end].arg = arg;
                atomic_flag_test_and_set(&local_pool->task_arry[end].task_ready);
                atomic_fetch_add_explicit(&local_pool->task_size, 1, memory_order_acq_rel);

                yexi_cond_signal(local_pool->pop_mutex_lock);
                atomic_flag_clear(&local_pool->push_Spin_lock); // unlock
            } else if (end == (local_pool->task_max_index)) {
                atomic_exchange_explicit(&local_pool->end, 0, memory_order_acq_rel);
                atomic_flag_clear(&local_pool->push_Spin_lock); // unlock
                goto Restart;
            } else {
                atomic_flag_clear(&local_pool->push_Spin_lock); // unlock
                goto Restart;
            }
        } else {
            atomic_flag_clear(&local_pool->push_Spin_lock);
            return YEXI_Statu_Unsuccess;
        }
    }
    return YEXI_Statu_Success;
}

INT_64 yexi_thread_pool_free(IN PVOID ptr_thread_pool) {
    return YEXI_Statu_Success;
}