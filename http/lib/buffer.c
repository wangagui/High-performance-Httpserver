#include "buffer.h"
#include "common.h"

const char* CRLF = "\r\n";

struct buffer* buffer_new() {
	struct buffer* buffer1 = malloc(sizeof(struct buffer));
	if(buffer1==NULL) return NULL;
	
	buffer1->data = malloc(INIT_BUFFER_SIZE);
	buffer1->total_size = INIT_BUFFER_SIZE;
	buffer1->readIndex = 0;
	buffer1->writeIndex = 0;
	return buffer1;
}

void buffer_free(struct buffer* buffer1) {
	free(buffer1->data);
	free(buffer1);
} 

int buffer_writeable_size(struct buffer* buffer1) {
	return buffer1->total_size - buffer1->writeIndex;
} 

int buffer_readable_size(struct buffer* buffer1) {
	return buffer1->writeIndex - buffer1->readIndex;
}

int buffer_front_spare_size(struct buffer* buffer1) {
	return buffer1->readIndex;
} 

void make_room(struct buffer* buffer1, int size) {
	if(buffer_writeable_size(buffer1) >= size) {
		/*说明尾部剩余的空间还够写的*/ 
		return ; 
	}
	/*头部剩余空间+尾部剩余空间 >= 容纳数据，则可以把数据向前拷贝*/ 
	if(buffer_front_spare_size(buffer1)+buffer_writeable_size(buffer1) >= size) {
		int readable = buffer_readable_size(buffer1);
		int i;	
		for(i=0; i < readable; i++) {
			memcpy(buffer1->data+i,buffer1->data+buffer1->readIndex+i,1);
		}
		buffer1->readIndex = 0;
		buffer1->writeIndex = readable;
	} else {
		/*扩容*/ 
		void *tmp = realloc(buffer1->data,buffer1->total_size+size);
		if(tmp == NULL) return ;
		buffer1->data = tmp;
		buffer1->total_size += size; 
	}
	 
}

int buffer_append(struct buffer* buffer1,void *data,int size) {
	if(data!=NULL) {
	make_room(buffer1,size);
	}
	/*拷贝数据*/ 
	memcpy(buffer1->data+buffer1->writeIndex,data,size);
	buffer1->writeIndex += size;
}

int buffer_append_char(struct buffer* buffer1,char data) {
	make_room(buffer1,1);
	/*拷贝数据*/ 
	buffer1->data[buffer1->writeIndex++] = data; 
}

int buffer_append_string(struct buffer* buffer1,char *data) {
	if(data!=NULL) {
		int size = strlen(data);
		buffer_append(buffer1,data,size);
	}
}

int buffer_read_from_socket(struct buffer* buffer1,int fd) {
	/*这里遇到的问题，从套接字读数据，我们并不清楚，会读到多少
	所以我们用到readv，开辟一个额外的缓冲区，把数据同时读到buffer和这个缓冲区里*/ 
	char temp_buffer[INIT_BUFFER_SIZE];
	struct iovec vec[2];
	int max_writeable = buffer_writeable_size(buffer1);
	vec[0].iov_base = buffer1->data + buffer1->writeIndex;
	vec[0].iov_len  = max_writeable;
	vec[1].iov_base = temp_buffer;
	vec[1].iov_len  = sizeof(temp_buffer);
	int actually_read = readv(fd,vec,2);
	if(actually_read <0) {
		return -1;
	} else if(actually_read <= max_writeable) {
		buffer1->writeIndex += actually_read;
	} else {
		buffer1->writeIndex = buffer1->total_size;
		buffer_append(buffer1,temp_buffer,actually_read - max_writeable); 
	}
	return actually_read;
}

char buffer_read_char(struct buffer* buffer1) {
	char c = buffer1->data[buffer1->readIndex];
	buffer1->readIndex++;
	return c; 
} 

char* buffer_find_CRLF(struct buffer* buffer1) {
	char* crlf = memmem(buffer1->data + buffer1->readIndex,buffer_readable_size(buffer1),CRLF,2);
	return crlf;
} 