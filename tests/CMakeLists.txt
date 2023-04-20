
include_directories(../include)

#target_include_directories(../lib)



ADD_EXECUTABLE(test main.cpp)


target_link_libraries(test PUBLIC YEXI_ThreadPool)

