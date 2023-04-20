#include "head.h"
#include "systemcallAPI.h"

#include <cstdio>
#include <iostream>
#include <ctime>
#include <chrono>
#include <pthread.h>
#include <unistd.h>
extern "C" {
#include "../include/ThreadPool.h"
}

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

PVOID thread_pool;

atomic_uint_fast64_t sum;

VOID task(PVOID) {
    struct timespec ts, ts1;
    ts.tv_nsec = 5000000; // 5ms
    ts.tv_sec = 0;
    nanosleep(&ts, nullptr);
    atomic_fetch_add_explicit(&sum, 1, memory_order_acq_rel);
}

PVOID pushtasks(PVOID da) {
    for (int i = 0; i < 1000; i++) {
        if (yexi_thread_pool_push(thread_pool, task, NULL)) {
            i--;

            struct timespec ts, ts1;
            ts.tv_nsec = 50000000; // 50ms
            ts.tv_sec = 0;
            nanosleep(&ts, nullptr);
        }
    }
    return nullptr;
}

using namespace std;

int main() {
    Init_Thread_Pool_Data data;
    data.Thread_max_size = 8;
    data.task_fast_size = 64;
    data.task_slow_size = 32;
    data.task_max_size = 1024 * 4;
    thread_pool = yexi_thread_pool_init(data);
    atomic_exchange_explicit(&sum, 0, memory_order_acq_rel);
    pthread_t thread[256];

    // Start measuring time
    auto begin = std::chrono::high_resolution_clock::now();

    {
        for (int i = 0; i < 20; i++) {
            INT_32 ret = pthread_create(&thread[i], NULL, pushtasks, nullptr);
        }
        PVOID da;
        for (int i = 0; i < 20; i++) {
            int res = pthread_join(thread[i], &da);
            if (res != 0) {
                printf("1：等待线程失败");
            }
        }
        
        //阻塞等待任务全部结束。
        Ptr_Pool local_pool = (Ptr_Pool)thread_pool;
        while (local_pool->task_size != 0) {
            sleep(5);
        };

        yexi_thread_pool_free(thread_pool);
    }

    // Stop measuring time and calculate the elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    printf("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);

    std::cout << sum << std::endl;
    return 0;
}