#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_

#include "common.h"
struct acceptor {
    int listen_port;
    int listen_fd;
};

struct acceptor* acceptor_init(int port);
#endif