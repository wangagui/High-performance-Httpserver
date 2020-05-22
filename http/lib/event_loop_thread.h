#ifndef EVENT_LOOP_THREAD_H_
#define EVENT_LOOP_THREAD_H_

#include <pthread.h>

struct event_loop_thread {
    struct event_loop *eventLoop;
    pthread_t thread_tid;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char * thread_name;
    long thread_count;           /* not yet configured */
};

/* 初始化操作 */
int event_loop_thread_init(struct event_loop_thread *, int );

/* 主线程调用，初始化一个子线程并运行 */
struct event_loop *event_loop_thread_start(struct event_loop_thread *);


#endif