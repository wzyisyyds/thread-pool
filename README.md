# Thread Pool
## 项目介绍
####  高性能线程池（基于循环数组的多读多写的无锁队列的实现） 

-  具备两个模式：低任务模式、高任务模式 
-  低任务模式：线程会进行休眠，多线程入队出队带锁，入队自旋锁，出队互斥锁 （方便休眠  ），从而实现一读一写的无锁队列。
-  高任务模式： 基于循环数组的多读多写 使用CAS 原子操作进行实现，避免多线程竞争的浪费。
-  模式切换： 任务数量，大于task_fast_size，启动高任务模式，小于 task_slow_size，切换回低任务模式

#### 测试用例tests文件下

## 使用说明
#### 构建环境
```cmake
cmake_minimum_required(VERSION 3.22)
#[[
    No LSB modules are available.
    Distributor ID: Ubuntu
    Description:    Ubuntu 22.04.2 LTS
    Release:        22.04
    Codename:       jammy

#Ubuntu clang version 14.0.0-1ubuntu1

#GNU Make 4.3

     #Linux version 5.19.0-35-generic (buildd@lcy02-amd64-020) 
     (x86_64-linux-gnu-gcc (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0,
     GNU ld (GNU Binutils for Ubuntu) 2.38) 
     #36~22.04.1-Ubuntu SMP PREEMPT_DYNAMIC Fri Feb 17 15:17:25 UTC 2
]]
```



---
#### 使用

1. 克隆项目 ，并且构建项目
```shell
cd thread-pool
mkdir build
cmake ..
make all
```
2. 将include目录和编译出来的静态库（lib目录下）加入你的项目
```c++

extern "C" {
#include "../include/ThreadPool.h"
}
//初始化线程池
//初始化Init_Thread_Pool_Data
//PVOID yexi_thread_pool_init(IN Init_Thread_Pool_Data data);
//参数为Init_Thread_Pool_Data，返回值为线程池结构指针。
{
    Init_Thread_Pool_Data data;
    data.Thread_max_size = 8;
    data.task_fast_size = 64;
    data.task_slow_size = 32;
    data.task_max_size = 1024 * 4;
    thread_pool = yexi_thread_pool_init(data);
}

//ptr_thread_pool 线程池结构指针 是yexi_thread_pool_init返回值
//IN VOID (*function)(PVOID)任务函数
//IN PVOID arg 任务函数的参数
{
    INT_64 yexi_thread_pool_push(IN PVOID ptr_thread_pool, IN VOID (*function)(PVOID), IN PVOID arg);
}


//销毁线程池
//参数为线程池结构指针。
{
    yexi_thread_pool_free(thread_pool);
}


```

