#ifndef BUFFER_H_
#define BUFFER_H_
/*  64KB = 65536B*/ 
#define INIT_BUFFER_SIZE 65536   

/*buffer 对象封装了缓冲区*/ 
struct buffer {
	char* data;           /*实际缓冲*/ 
	int readIndex;        /*缓冲读取位置*/ 
	int writeIndex;       /*缓冲写入位置*/ 
	int total_size;       /*总的大小 */ 
}; 

struct buffer* buffer_new();

void buffer_free(struct buffer* buffer);

int buffer_writeable_size(struct buffer* buffer);

int buffer_readable_size(struct buffer* buffer);
/*获得缓冲区头部空闲的空间，就是已经被读走的区域 */
int buffer_front_spare_size(struct buffer* buffer);

/*写数据到buffer*/
int buffer_append(struct buffer* buffer, void *data, int size);

/*写字符到buffer*/
int buffer_append_char(struct buffer* buffer, char data);

/*往buffer里写字符串*/
int buffer_append_string(struct buffer* buffer,char* data); 
/*读socket，写入buffer*/
int buffer_read_from_socket(struct buffer* buffer,int fd);
/*读buffer*/
char buffer_read_char(struct buffer* buffer);
/*查询buffer*/
char* buffer_find_CRLF(struct buffer* buffer); 


#endif 
