#include "head.h"
#include "systemcallAPI.h"

#include <iostream>
# include<ctime>
#include <pthread.h>
extern "C" {
#include "../include/ThreadPool.h"
}

PVOID thread_pool;

VOID task(PVOID) {
    struct timespec ts, ts1;
    int count = 0;

    ts.tv_nsec = 50, 000, 000; 
    ts.tv_sec = 0;
    nanosleep(&ts, nullptr);
}

PVOID pushtasks(PVOID da) {
    for (int i = 0; i < 1000; i++) {
        if (yexi_thread_pool_push(thread_pool, task, NULL)) {
        i--;
        }
    }
    return nullptr;
}

using namespace std;

int main() {
    Init_Thread_Pool_Data data;
    data.Thread_max_size = 4;
    data.task_fast_size = 16;
    data.task_slow_size = 8;
    data.task_max_size = 1024*8;
    thread_pool = yexi_thread_pool_init(data);


    clock_t  Begin, End;
    double duration;

    pthread_t thread[300];
 
    Begin = clock();//开始计时
     
    for (int i = 0; i < 20; i++) {
        INT_32 ret = pthread_create(&thread[i], NULL, pushtasks, nullptr);
    }
    PVOID da;
    for (int i = 0; i < 20; i++) {
        pthread_join(thread[i],&da);
    }
    

    yexi_thread_pool_free(thread_pool);


    End = clock();//结束计时
 
    duration = double(End - Begin) / CLOCKS_PER_SEC; 
    cout << "tick=" << double(End - Begin) << endl; //运行函数所打的点数
    cout << "运行时间: " << duration << "s" << endl; //s



    std::cout << "hello word" << std::endl;
    return 0;
}