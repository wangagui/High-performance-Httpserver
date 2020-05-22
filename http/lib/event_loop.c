#include <assert.h>
#include "event_loop.h"
#include "common.h"
#include "log.h"
#include "event_dispatcher.h"
#include "channel.h"


int channel_event_activate(struct event_loop *eventLoop, int fd, int revents) {
    struct channel_map *map = eventLoop->channelMap;
    wag_msgx("activate channel fd == %d, revents=%d, %s", fd, revents, eventLoop->thread_name);

    if( fd<0 ) {
        return 0;
    }
    if(fd >= map->nentries) return (-1);

    struct channel *channel = map->entries[fd];
    assert(fd == channel->fd);

    if (revents & (EVENT_READ)) {
        if (channel->eventReadCallback) channel->eventReadCallback(channel->data);
    }
    if (revents & (EVENT_WRITE)) {
        if (channel->eventWriteCallback) channel->eventWriteCallback(channel->data);
    }

    return 0;
}

int event_loop_run(struct event_loop *eventLoop) {
    assert(eventLoop != NULL);

    struct event_dispatcher *dispatcher = eventLoop->eventDispatcher;
    /* 如果不是eventLoop的所属线程调用此函数，直接结束该线程 */
    if(eventLoop->owner_thread_id != pthread_self()) {
        exit(1);
    }

    wag_msgx("event loop run, %s",eventLoop->thread_name);

    struct timeval timeval;
    timeval.tv_sec = 1;

    while(!eventLoop->quit) {
        /* 获得活动的channel */
        dispatcher->dispatch(eventLoop,&timeval);

        /* 处理待定channel */
        event_loop_handle_pending_channel(eventLoop);
    }
    
    wag_msgx("event loop end, %s",eventLoop->thread_name);      
    return 0;
}


int event_loop_handle_pending_channel(struct event_loop *eventLoop) {
    /* 先拿锁，在做事 */
    pthread_mutex_lock(&eventLoop->mutex);
    eventLoop->is_handle_pending = 1;

    struct channel_element *channelElement = eventLoop->pending_head;   
    while(channelElement != NULL) {
        
        struct channel *channel = channelElement->channel;
        int fd = channel->fd;

        if(channelElement->type == 1) {
            event_loop_handle_pending_add(eventLoop,fd,channel);
        } else if(channelElement->type == 2) {
            event_loop_handle_pending_remove(eventLoop,fd,channel);
        } else if(channelElement->type == 3) {
            event_loop_handle_pending_update(eventLoop,fd,channel);
        }

        channelElement = channelElement->next;
    }

    eventLoop->pending_head = eventLoop->pending_tail = NULL;
    eventLoop->is_handle_pending = 0;

    //release the lock
    pthread_mutex_unlock(&eventLoop->mutex);

    return 0;
}

int event_loop_handle_pending_add(struct event_loop *eventLoop,int fd,struct channel *channel) {
    wag_msgx("add channel fd == %d, %s", fd, eventLoop->thread_name);
    struct channel_map *map = eventLoop->channelMap;

    if(fd<0) {
        return 0;
    }

    if(fd >= map->nentries) {
        if( map_make_space(map,fd,sizeof(struct channel *)) == -1) {
            return (-1);
        }
    }

    if((map)->entries[fd] == NULL) {
        map->entries[fd] = channel;
        
        struct event_dispatcher *eventDispatcher = (struct event_dispatcher *)eventLoop->eventDispatcher;
        eventDispatcher->add(eventLoop,channel);
        return 1;
    } 
    return 0;
 }

 // in the i/o thread
int event_loop_handle_pending_remove(struct event_loop *eventLoop, int fd, struct channel *channel1) {
    struct channel_map *map = eventLoop->channelMap;
    assert(fd == channel1->fd);

    if (fd < 0)
        return 0;

    if (fd >= map->nentries)
        return (-1);

    struct channel *channel2 = (struct channel *)map->entries[fd];

    //update dispatcher(multi-thread)here
    struct event_dispatcher *eventDispatcher = (struct event_dispatcher *)eventLoop->eventDispatcher;

    int retval = 0;
    if (eventDispatcher->del(eventLoop, channel2) == -1) {
        retval = -1;
    } else {
        retval = 1;
    }

    map->entries[fd] = NULL;
    return retval;
}

// in the i/o thread
int event_loop_handle_pending_update(struct event_loop *eventLoop, int fd, struct channel *channel) {
    wag_msgx("update channel fd == %d, %s", fd, eventLoop->thread_name);
    struct channel_map *map = eventLoop->channelMap;

    if (fd < 0)
        return 0;

    if ((map)->entries[fd] == NULL) {
        return (-1);
    }

    //update channel
    struct event_dispatcher *eventDispatcher = (struct event_dispatcher *)eventLoop->eventDispatcher;
    eventDispatcher->update(eventLoop, channel);
}


