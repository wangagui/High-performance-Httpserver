#ifndef UTILS_H_
#define UTILS_H_

#include "event_loop.h"

void assertInSameThread(struct event_loop* eventLoop);


int isInSameThread(struct event_loop* eventLoop);
#endif