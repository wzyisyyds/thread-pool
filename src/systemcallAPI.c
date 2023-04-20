#include "../include/systemcallAPI.h"

/*local structs*/
typedef struct Mutex_Struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;

} Mutex, *Ptr_Mutex;

/*open API*/
PVOID yexi_mutex_create() {
    Ptr_Mutex ptr = malloc(sizeof(Mutex));
    if (!pthread_mutex_init(&ptr->mutex, NULL)) { return NULL; }
    if (!pthread_cond_init(&ptr->cond, NULL)) { return NULL; }
    return ptr;
}

INT_64 yexi_mutex_free(PVOID prt_Mutex) {
    if (!prt_Mutex) { return YEXI_Statu_Unsuccess; }
    free(prt_Mutex);
    return YEXI_Statu_Success;
}

INT_64 yexi_mutex_lock(PVOID prt_Mutex) {
    if (!prt_Mutex) { return YEXI_Statu_Unsuccess; }
    Ptr_Mutex ptr = prt_Mutex;
    if (pthread_mutex_lock(&ptr->mutex)) { return YEXI_Statu_Unsuccess; }
    return YEXI_Statu_Success;
}

INT_64 yexi_mutex_unlock(PVOID prt_Mutex) {
    if (!prt_Mutex) { return YEXI_Statu_Unsuccess; }
    Ptr_Mutex ptr = prt_Mutex;
    if (pthread_mutex_unlock(&ptr->mutex)) { return YEXI_Statu_Unsuccess; }
    return YEXI_Statu_Success;
}

INT_64 yexi_cond_wait(PVOID prt_Mutex) {
    if (!prt_Mutex) { return YEXI_Statu_Unsuccess; }
    Ptr_Mutex ptr = prt_Mutex;
    if (pthread_cond_wait(&ptr->cond, &ptr->mutex)) { return YEXI_Statu_Unsuccess; }
    yexi_mutex_unlock(ptr);
    return YEXI_Statu_Success;
}

INT_64 yexi_cond_signal(PVOID prt_Mutex) {
    if (!prt_Mutex) { return YEXI_Statu_Unsuccess; }
    Ptr_Mutex ptr = prt_Mutex;
    if (pthread_cond_signal(&ptr->cond)) { return YEXI_Statu_Unsuccess; }
    return YEXI_Statu_Success;
}

INT_64 yexi_cond_broadcast(PVOID prt_Mutex) {
    if (!prt_Mutex) { return YEXI_Statu_Unsuccess; }
    Ptr_Mutex ptr = prt_Mutex;
    if (pthread_cond_broadcast(&ptr->cond)) { return YEXI_Statu_Unsuccess; }
    return YEXI_Statu_Success;
}

VOID yexi_thread_exit() {
    pthread_exit(0);
}

VOID yexi_pthread_create_and_detach(void *(*start_routine)(void *), void *arg) {
    pthread_t thread;
    INT_32 ret = pthread_create(&thread, NULL, start_routine, arg);
    pthread_detach(thread);
}

