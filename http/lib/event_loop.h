#ifndef EVENT_LOOP_H_
#define EVENT_LOOP_H_

#include <pthread.h>
#include "channel.h"
#include "event_dispathcer.h"
#include "common.h"

/* 这里我们用epoll做事件分发器 ，当然我们可以有更多选择，比如用poll */
extern const struct event_dispathcer epoll_dispatcher;

struct channel_element {
    int type; //1:add   2: delete
    struct channel *channel;
    struct channel_element *next;
};
/*
*   这里说一下event_loop这个数据结构，它和线程绑定，它包含了：
*   1. 事件分发器 eventDispatcher
*   2. channel_map 对注册到该eventLoop上的事件做映射，快速找到回调函数
*   3. 线程控制相关： 锁，条件变量，线程号
*   4. 一对相互连接的socket，初始化好就是可以互相通信的，类似pipe
*/
struct event_loop {
    int quit;
    /*
    *   eventDispatcher与event_dispatcher_data的关系为:
    *   前者是对epoll的操作的封装
    *   后者是对epoll数据的封装
    */
    const struct event_dispatcher *eventDispatcher;
    void *event_dispatcher_data;

    struct channel_map *channelMap;

    int is_handle_pending;
    struct channel_element *pending_head;
    struct channel_element *pending_tail;

    pthread_t owner_thread_id;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    /* 两个互相连接的套接字描述符，主reactor通知sub-reactor用？ */
    int socketPair[2];

    char *thread_name;

};



struct event_loop *event_loop_init();

struct event_loop *event_loop_init_with_name(char * thread_name);

/*  对发生在channel上的事件执行回调
*/
int channel_event_activate(struct event_loop *eventLoop, int fd, int revents);

/*
    1. dispatch
    2. handle_pending_channel
*/
int event_loop_run(struct event_loop *eventLoop);

void event_loop_wakeup(struct event_loop *eventLoop);

int event_loop_add_channel_event(struct event_loop *eventLoop, int fd, struct channel *channel1);

int event_loop_remove_channel_event(struct event_loop *eventLoop, int fd, struct channel *channel1);

int event_loop_update_channel_event(struct event_loop *eventLoop, int fd, struct channel *channel1);


int event_loop_handle_pending_add(struct event_loop *eventLoop, int fd, struct channel *channel);

int event_loop_handle_pending_remove(struct event_loop *eventLoop, int fd, struct channel *channel);

int event_loop_handle_pending_update(struct event_loop *eventLoop, int fd, struct channel *channel);
#endif