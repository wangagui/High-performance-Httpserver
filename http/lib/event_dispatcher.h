#ifndef EVENT_DISPATCHER_H_
#define EVENT_DISPATCHER_H_

#include "channel.h"

struct event_dispatcher {
    /* event_dispatcher为事件分发器，为select/poll/epoll的抽象*/
    const char *name;

    /**  初始化函数 */+
    void *(*init)(struct event_loop * eventLoop);

    /** 通知dispatcher新增一个channel事件*/
    int (*add)(struct event_loop * eventLoop, struct channel * channel);

    /** 通知dispatcher删除一个channel事件*/
    int (*del)(struct event_loop * eventLoop, struct channel * channel);

    /** 通知dispatcher更新channel对应的事件*/
    int (*update)(struct event_loop * eventLoop, struct channel * channel);

    /** 实现事件分发，然后调用event_loop的event_activate方法执行callback*/
    int (*dispatch)(struct event_loop * eventLoop, struct timeval *);

    /** 清除数据 */
    void (*clear)(struct event_loop * eventLoop);   
};


#endif