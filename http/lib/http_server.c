#include <assert.h>
#include "http_server.h"


/* 连接建立 回调 */
int http_onConnectionCompleted(struct tcp_connection *tcpConnection) {
    wag_msgx("connection completed");
    struct http_request *httpRequest = http_request_new();
    tcpConnection->request = httpRequest;
    return 0;
}

int http_onMessage(struct buffer *input, struct tcp_connection *tcpConnection) {
        wag_msgx("get message from tcp connection %s", tcpConnection->name);

        struct http_request *httpRequest = (struct http_request *)tcpConnection->request;
        struct http_server *httpServer = (struct http_server *)tcpConnection->data;

        
}

int http_onWriteCompleted(struct tcp_connection *tcpConnection) {

}

int http_onConnectionClosed(struct tcp_connection *tcpConnection) {

}
struct http_server *http_server_new(struct event_loop *eventLoop, int port,
                                    request_callback requestCallback,
                                    int threadNum) {
    struct http_server *httpServer = malloc(sizeof(struct http_server));
    httpServer->requestCallback = requestCallback;
    //初始化tcpServer
    struct acceptor *acceptor = acceptor_init(SERV_PORT);
    httpServer->tcpServer = tcp_server_init(eventLoop, acceptor, http_onConnectionCompleted, http_onMessage,
                                            http_onWriteCompleted,
                                            http_onConnectionClosed, threadNum);

    // for callback
    httpServer->tcpServer->data = httpServer;

    return httpServer;
}

void http_server_start(struct http_server *httpServer) {
    tcp_server_start(httpServer->tcpServer);
}