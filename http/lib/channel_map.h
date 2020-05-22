#ifndef CHANNEL_MAP_H_
#define CHANNEL_MAP_H_

#include "channel.h"
/*
*   channel映射表，socket-->channel
*/

struct channel_map {
    /* 这里void*型，方便我们后续扩展，所以扩容时要传入大小作为参数 */
    void **entries;

    /* 可用的entries数量 */
    int nentries;  
};

int map_make_space(struct channel_map *map, int slot, int msize);

void map_init(struct channel_map *map);

void map_clear(struct channel_map *map);
#endif
