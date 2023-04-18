SET(CMAKE_C_COMPILER /usr/bin/clang)
SET(CMAKE_CXX_COMPILER /usr/bin/clang++)
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
SET(CMAKE_C_STANDARD 11)
SET(CMAKE_CXX_STANDARD 17)

cmake_minimum_required(VERSION 3.22)


PROJECT(ThreadPool C CXX)


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


ADD_SUBDIRECTORY(src LIBRARY_OUTPUT_PATH)

ADD_SUBDIRECTORY(tests EXECUTABLE_OUTPUT_PATH)


