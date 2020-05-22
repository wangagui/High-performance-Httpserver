#ifndef EVENT_LOOP_THREAD_POOL_H_
#define EVENT_LOOP_THREAD_POOL_H_

#include "event_loop.h"
#include "event_loop_thread.h"


/*
*   main reacotr ----> accepotr 线程
*   需要解决的问题：
*       1.主线程如何判断子线程已经完成初始化并启动，继续执行下去
*       2.如何协调主线程数据写入给子线程
*   子线程是event_loop线程，它阻塞在dispatch上
*/


struct event_loop_thread_pool {
    /* 创建线程池的主线程 */
    struct event_loop *mainloop;
    /* 是否已经启动 */
    int started;
    /* 线程数目 */
    int thread_number;
    /* 数组指针，指向创建的event_loop_thread数组 */
    struct event_loop_thread *eventLoopThreads;

    /* 用来决定选择哪个线程服务 */
    int position;
};


struct event_loop_thread_pool *event_loop_thread_pool_new(struct event_loop *mainloop, int threadNumber);

void event_loop_thread_pool_start(struct event_loop_thread_pool*);

/* 选择线程算法 */
struct event_loop *event_loop_thread_pool_get_loop(struct event_loop_thread_pool *);

#endif