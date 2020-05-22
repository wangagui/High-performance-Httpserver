#include <assert.h>
#include "event_loop.h"
#include "common.h"
#include "log.h"
#include "event_dispatcher.h"
#include "channel.h"

int event_loop_do_channel_event(struct event_loop *eventLoop,int fd,struct channel* channle1,int type) {
    /* 先获得锁 */
    pthread_mutex_lock(&eventLoop->mutex);
    assert(eventLoop->is_handle_pending == 0);
    event_loop_add_channel_to_pending(eventLoop,fd,channel1,type);
    /*加上pending之后就可以释放锁了 */
    pthread_mutex_unlock(&eventLoop->mutex);

    if(!isInSameThread(eventLoop)) {
        event_loop_wakeup(eventLoop);
    } else {
        event_loop_handle_pending_channel(eventLoop);
    }
}   

int event_loop_add_channel_event(struct event_loop *eventLoop, int fd, struct channel *channel1) {
    return event_loop_do_channel_event(eventLoop, fd, channel1, 1);
}

int event_loop_remove_channel_event(struct event_loop *eventLoop, int fd, struct channel *channel1) {
    return event_loop_do_channel_event(eventLoop, fd, channel1, 2);
}

int event_loop_update_channel_event(struct event_loop *eventLoop, int fd, struct channel *channel1) {
    return event_loop_do_channel_event(eventLoop, fd, channel1, 3);
}


void event_loop_add_channel_to_pending(struct event_loop *eventLoop,int fd,struct channel *channle1, int type) {
    /* 将channel加入pending list */
    struct channel_element *channelElement = (struct channel_element *)malloc(sizeof(struct channel_element));
    channelElement->channel = channel1;
    channelElement->type = type;
    channelElement->next = NULL;
    /* 如果当前pending为空 */
    if (eventLoop->pending_head == NULL) {
        eventLoop->pending_head = eventLoop->pending_tail = channelElement;
    } else {
        eventLoop->pending_tail->next = channelElement;
        eventLoop->pending_tail = channelElement;
    }
}

/*  如果唤醒一个线程，这是个有趣的问题，还记得socketPair吗
*   向socketPair[0]写，socketPair[1]可读
*   我们在一开始初始化的时候就将socketPair[1]以监听读的方式注册进pending_list里了
*/
void event_loop_wakeup(struct event_loop *eventLoop) {
    char one = 'a';
    ssize_t n = write(eventLoop->socketPair[0], &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERR("wakeup event loop thread failed");
    }
}
/* 响应以下就可以了，目的是让线程有任务而唤醒 */
int handleWakeup(void *data) {
    struct event_loop *eventLoop = (struct event_loop *) data;
    char one;
    ssize_t n = read(eventLoop->socketPair[1], &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERR("handleWakeup  failed");
    }
    wag_msgx("wakeup, %s", eventLoop->thread_name);
}