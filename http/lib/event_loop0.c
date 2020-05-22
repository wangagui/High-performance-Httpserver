#include <assert.h>
#include "event_loop.h"
#include "common.h"
#include "log.h"
#include "event_dispatcher.h"
#include "channel.h"


struct event_loop *event_loop_init() {
    return event_loop_init_with_name(NULL);
}

struct event_loop *event_loop_init_with_name(char *thread_name) {
    struct event_loop *eventLoop = malloc(sizeof(struct event_loop));
    /* 初始化锁，条件变量 */
    pthread_mutex_init(&eventLoop->mutex, NULL);
    pthread_cond_init(&eventLoop->cond, NULL);
    /* 如果未指定名字，说明是main-reacotr */
    if (thread_name != NULL) {
        eventLoop->thread_name = thread_name;
    } else {
        eventLoop->thread_name = "main thread";
    }

    eventLoop->quit = 0;
    eventLoop->channelMap = malloc(sizeof(struct channel_map));
    map_init(eventLoop->channelMap);

    //用epoll做分发器
    eventLoop->eventDispatcher = &epoll_dispatcher;

    eventLoop->event_dispatcher_data = eventLoop->eventDispatcher->init(eventLoop);
    /* 现在还不知道 pending 是啥意思 */
    eventLoop->is_handle_pending = 0;
    eventLoop->pending_head = NULL;
    eventLoop->pending_tail = NULL;

    eventLoop->owner_thread_id = pthread_self();
    /* 创建一对互相连接的套接字描述符 */
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, eventLoop->socketPair) < 0) {
        LOG_ERR("socketpair set fialed");
    }

    struct channel *channel = channel_new(eventLoop->socketPair[1], EVENT_READ, handleWakeup, NULL, eventLoop);
    event_loop_add_channel_event(eventLoop, eventLoop->socketPair[1], channel);


    return eventLoop;
}