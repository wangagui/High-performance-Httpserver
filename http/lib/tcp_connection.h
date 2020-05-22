#ifndef TCP_CONNECTION_H_
#define TCP_CONNECTION_H_

#include "event_loop.h"
#include "channel.h"
#include "buffer.h"
#include "tcp_server.h"

struct tcp_connection{
    struct event_loop *eventLoop;
    struct channel *channel;
    struct buffer *input_buffer;
    struct buffer *output_buffer;
    char *name;
    
    connection_completed_call_back connectionCompletedCallBack;
    message_call_back messageCallBack;
    write_completed_call_back writeCompletedCallBack;
    connection_closed_call_back connectionClosedCallBack;

    void * data; //for callback use: http_server
    void * request; // for callback use
    void * response; // for callback use
};

struct tcp_connection *
tcp_connection_new(int fd, struct event_loop *eventLoop, connection_completed_call_back connectionCompletedCallBack,
                   connection_closed_call_back connectionClosedCallBack,
                   message_call_back messageCallBack, write_completed_call_back writeCompletedCallBack);

//应用层调用入口
int tcp_connection_send_data(struct tcp_connection *tcpConnection, void *data, int size);

//应用层调用入口
int tcp_connection_send_buffer(struct tcp_connection *tcpConnection, struct buffer * buffer);

void tcp_connection_shutdown(struct tcp_connection * tcpConnection);
//int tcp_connection_append_buffer(struct tcp_connection *tcpConnection);

#endif