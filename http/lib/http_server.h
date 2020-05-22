#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include "common.h"
#include "tcp_server.h"
#include "http_request.h"
#include "http_response.h"

typedef int (*request_callback)(struct http_request *httpRequest, struct http_response *httpReponse);

/* http_server 这个结构体是TCPserver的又一次封装
*  更方便调用而已
*/

struct http_server {
    struct TCPserver *tcpServer;
    request_callback requestCallback;
};

struct http_server *http_server_new(struct event_loop *eventLoop, int port,
                                    request_callback requestCallback,
                                    int threadNum);

void http_server_start(struct http_server *httpServer);

#endif

