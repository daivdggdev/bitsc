#ifndef _LOG_H
#define _LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>

#define log(fd, level, errnum, format, x) do { \
                 char fmt[1024] = {0}; \
                 snprintf(fmt, 1024, "[fun: %s : %d] %s", __func__, __LINE__, format); \
                 pps_log4cxx(fd, level, errnum, fmt, x); \
                 } while(0);

#define logoi(format, x)    log(STDOUT, MSG_INFO, 0, format, x)
#define logow(format, x)    log(STDOUT, MSG_WARNING, 0, format, x)
#define logoe(format, x)    log(STDOUT, MSG_ERROR, 0, format, x)

#define logfi(fd, format, x)    log(fd, MSG_INFO, 0, format, x)
#define logfw(fd, format, x)    log(fd, MSG_WARNING, 0, format, x)
#define logfe(fd, format, x)    log(fd, MSG_ERROR, 0, format, x)

typedef enum __log_level {
	MSG_INFO,
	MSG_WARNING,
	MSG_ERROR
} log_level;

void pps_log4cxx(int fd, log_level level, int errnum, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
