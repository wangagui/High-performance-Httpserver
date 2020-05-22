#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

struct request_header {
    char *key;
    char *value;
};
/* 定义http的状态 */
enum http_request_state {
    REQUEST_STATUS,     // 等待解析状态行
    REQUEST_HEADERS,    // 等待解析请求头
    REQUEST_BODY,       // 等待解析请求体
    REQUEST_DONE        // 解析完成
};

struct http_request {
    char *version;
    char *method;
    char *url;
    enum http_request_state current_state;
    struct http_header *request_header;
    int request_header_number;
};


/* 初始化一个request对象 */
struct http_request *http_request_new();

/* 清除一个request对象 */
void http_request_clear(struct http_request *httpRequest);

/* 重置一个http对象 */
void http_request_reset(struct http_request *httpRequest);

/* 给request增加header */
void http_request_add_header(struct http_request *httpRequest , char *key, char *value);

/* 根据key的值获取header */
char *http_request_get_header(struct http_request *httpRequest,char *key);

/* 解析当前状态 */
enum http_request_state http_request_current_state(struct http_request *httpRequest);

/* 根据request请求决定是否关闭服务器 -> 客户端单向连接 */
int http_request_close_connection(struct http_request *httpRequest);


#endif