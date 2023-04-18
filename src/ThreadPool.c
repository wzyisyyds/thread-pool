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

    atomic_flag ThreadPool_mode;

    atomic_flag ThreadPool_KILL;

    atomic_flag push_Spin_lock;
    PVOID pop_mutex_lock;

    UINT_64 Thread_max_size;
    atomic_uint_fast64_t Thread_size;
    atomic_uint_fast64_t Thread_run_size;

    atomic_uint_fast64_t start;
    atomic_uint_fast64_t end;

    UINT_64 task_fast_size;
    UINT_64 task_slow_size;

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

        } else {
            yexi_mutex_lock(local_pool->pop_mutex_lock);
            start = atomic_load_explicit(&local_pool->start, memory_order_seq_cst);
            if (start >= local_pool->task_max_index) {
                atomic_exchange_explicit(&local_pool->start, 0, memory_order_acq_rel);
            }
            yexi_mutex_unlock(local_pool->pop_mutex_lock);
            goto Restart;
        }
    } else {
        yexi_mutex_lock(local_pool->pop_mutex_lock);

        if (local_pool->task_size <= 0) {
            yexi_mutex_unlock(local_pool->pop_mutex_lock);
            atomic_fetch_sub_explicit(&local_pool->Thread_run_size, 1, memory_order_acq_rel);
            yexi_cond_wait(local_pool->pop_mutex_lock);
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
        } else {
            start = atomic_load_explicit(&local_pool->start, memory_order_seq_cst);
            if (start >= local_pool->task_max_index) {
                atomic_exchange_explicit(&local_pool->start, 0, memory_order_acq_rel);
            }
            yexi_mutex_unlock(local_pool->pop_mutex_lock);
            goto Restart;
        }
        yexi_mutex_unlock(local_pool->pop_mutex_lock);
    }

    function(arg);
    return YEXI_Statu_Success;
}

static PVOID yexi_thread_task(IN PVOID ptr_thread_pool) {
    Ptr_Pool local_pool = ptr_thread_pool;
    atomic_fetch_add_explicit(&local_pool->Thread_size, 1, memory_order_acq_rel);
    atomic_fetch_add_explicit(&local_pool->Thread_run_size, 1, memory_order_acq_rel);
    while (1) {
        yexi_thread_pool_pop(local_pool);

        if (local_pool->task_size < local_pool->task_slow_size) {
            atomic_flag_clear(&local_pool->ThreadPool_mode);
        }
        if (local_pool->ThreadPool_KILL._Value) {
            atomic_fetch_sub_explicit(&local_pool->Thread_run_size, 1, memory_order_acq_rel);
            atomic_fetch_sub_explicit(&local_pool->Thread_size, 1, memory_order_acq_rel);
            yexi_thread_exit();
        }
    }
    return YEXI_Statu_Success;
}

/*open API*/

PVOID yexi_thread_pool_init(IN Init_Thread_Pool_Data data) {
    Ptr_Pool pool = malloc(sizeof(Pool));
    pool->task_fast_size = data.task_fast_size;
    pool->task_slow_size = data.task_slow_size;
    pool->task_max_size = data.task_max_size;
    pool->task_max_index = pool->task_max_size + 1023;
    pool->Thread_max_size = data.Thread_max_size;
    pool->self = pool;

    pool->pop_mutex_lock = yexi_mutex_create();

    pool->task_arry = malloc(sizeof(Task) * (pool->task_max_index + 1));

    for (int i = 0; i < pool->Thread_max_size; i++) {
        yexi_pthread_create_and_detach(yexi_thread_task, pool->self);
    }
    return pool->self;
}

INT_64 yexi_thread_pool_push(IN PVOID ptr_thread_pool, IN VOID (*function)(PVOID), IN PVOID arg) {
    Ptr_Pool local_pool = ptr_thread_pool;

Restart: // loop
    if (local_pool->ThreadPool_KILL._Value) {
        return YEXI_Statu_Unsuccess;
    }
    if (local_pool->task_size > local_pool->task_max_size) {
        return YEXI_Statu_Unsuccess;
    }

    if (local_pool->ThreadPool_mode._Value) {
        UINT_64 end = atomic_fetch_add_explicit(&local_pool->end, 1, memory_order_acq_rel);
        if (end < (local_pool->task_max_index)) {
            local_pool->task_arry[end].task_fun = function;
            local_pool->task_arry[end].arg = arg;
            atomic_flag_test_and_set(&local_pool->task_arry[end].task_ready);
            atomic_fetch_add_explicit(&local_pool->task_size, 1, memory_order_acq_rel);
        } else {
            while (atomic_flag_test_and_set(&local_pool->push_Spin_lock)) {}; // lock
            end = atomic_load_explicit(&local_pool->end, memory_order_seq_cst);
            if (end >= local_pool->task_max_index) {
                atomic_exchange_explicit(&local_pool->end, 0, memory_order_acq_rel);
            }
            atomic_flag_clear(&local_pool->push_Spin_lock); // unlock
            goto Restart;
        }

    } else {
        while (atomic_flag_test_and_set(&local_pool->push_Spin_lock)) {}; // Spin_lock
        UINT_64 end = atomic_fetch_add_explicit(&local_pool->end, 1, memory_order_acq_rel);
        if (end < (local_pool->task_max_index)) {
            local_pool->task_arry[end].task_fun = function;
            local_pool->task_arry[end].arg = arg;
            atomic_flag_test_and_set(&local_pool->task_arry[end].task_ready);
            atomic_fetch_add_explicit(&local_pool->task_size, 1, memory_order_acq_rel);

            yexi_cond_signal(local_pool->pop_mutex_lock);
            atomic_flag_clear(&local_pool->push_Spin_lock); // unlock
        } else {
            end = atomic_load_explicit(&local_pool->end, memory_order_seq_cst);
            if (end >= local_pool->task_max_index) {
                atomic_exchange_explicit(&local_pool->end, 0, memory_order_acq_rel);
            }
            atomic_flag_clear(&local_pool->push_Spin_lock); // unlock
            goto Restart;
        }
    }

    if (local_pool->task_size > local_pool->task_fast_size) {
        atomic_flag_test_and_set(&local_pool->ThreadPool_mode);
    }
    return YEXI_Statu_Success;
}

INT_64 yexi_thread_pool_free(IN PVOID ptr_thread_pool) {
    Ptr_Pool local_pool = ptr_thread_pool;
    while (local_pool->task_size != 0) {
    };
    atomic_flag_test_and_set(&local_pool->ThreadPool_KILL);
    while (local_pool->Thread_size != 0) {
        yexi_cond_broadcast(local_pool->pop_mutex_lock);
        sleep(5);
    }

    yexi_mutex_free(local_pool->pop_mutex_lock);

    free(local_pool->task_arry);
    free(local_pool->self);
    return YEXI_Statu_Success;
}