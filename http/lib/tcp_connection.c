#include "tcp_connection.h"
#include "utils.h"

struct tcp_connection *
tcp_connection_new(int connected_fd, struct event_loop *eventLoop,
                   connection_completed_call_back connectionCompletedCallBack,
                   connection_closed_call_back connectionClosedCallBack,
                   message_call_back messageCallBack, write_completed_call_back writeCompletedCallBack) {
    struct tcp_connection *tcpConnection = malloc(sizeof(struct tcp_connection));
    tcpConnection->writeCompletedCallBack = writeCompletedCallBack;
    tcpConnection->messageCallBack = messageCallBack;
    tcpConnection->connectionCompletedCallBack = connectionCompletedCallBack;
    tcpConnection->connectionClosedCallBack = connectionClosedCallBack;
    tcpConnection->eventLoop = eventLoop;
    tcpConnection->input_buffer = buffer_new();
    tcpConnection->output_buffer = buffer_new();


    char *buf = malloc(16);
    sprintf(buf, "connection-%d\0", connected_fd);
    tcpConnection->name = buf;

    // add event read for the new connection
    struct channel *channel1 = channel_new(connected_fd, EVENT_READ, handle_read, handle_write, tcpConnection);
    tcpConnection->channel = channel1;

    //connectionCompletedCallBack callback
    if (tcpConnection->connectionCompletedCallBack != NULL) {
        tcpConnection->connectionCompletedCallBack(tcpConnection);
    }

    event_loop_add_channel_event(tcpConnection->eventLoop, connected_fd, tcpConnection->channel);
    return tcpConnection;
}

int handle_connection_closed(struct tcp_connection *tcpConnection) {
    struct event_loop *eventLoop = tcpConnection->eventLoop;
    struct channel *channel = tcpConnection->channel;
    event_loop_remove_channel_event(eventLoop, channel->fd, channel);
    if (tcpConnection->connectionClosedCallBack != NULL) {
        tcpConnection->connectionClosedCallBack(tcpConnection);
    }
}

int handle_read(void *data) {
    struct tcp_connection *tcpConnection = (struct tcp_connection *) data;
    struct buffer *input_buffer = tcpConnection->input_buffer;
    struct channel *channel = tcpConnection->channel;

    if (buffer_socket_read(input_buffer, channel->fd) > 0) {
        //应用程序真正读取Buffer里的数据
        if (tcpConnection->messageCallBack != NULL) {
            tcpConnection->messageCallBack(input_buffer, tcpConnection);
        }
    } else {
        handle_connection_closed(tcpConnection);
    }
}

