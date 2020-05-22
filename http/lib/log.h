#ifndef LOG_H_
#define LOG_H_
#include<stdarg.h>

#define LOG_DEBUG_TYPE 0
#define LOG_MSG_TYPE 1
#define LOG_WARN_TYPE 2
#define LOG_ERR_TYPE 3

void wag_log(int severity, const char *msg);
void wag_logx(int severity, const char *errstr, const char *fmt, va_list ap);
void wag_msgx(const char *fmt, ...);
void wag_debugx(const char *fmt, ...);


void error(int status,int err,char *fmt,...);

#define LOG_MSG(msg) wag_log(LOG_MSG_TYPE, msg)
#define LOG_ERR(msg) wag_log(LOG_ERR_TYPE, msg)
#endif