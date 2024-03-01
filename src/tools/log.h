#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#define LOG_VER "0.0.0"

enum {
  TRACE,
  DEBUG,
  INFO,
  WARN,
  ERROR,
  FATAL
};

typedef struct {
  va_list ap;
  const char *fmt;
  const char *file;
  struct tm *time;
  void *udata;
  int line;
  int level;
} logEvent;

typedef void (*logLogFn)(logEvent *ev);
typedef void (*logLockFn)(bool lock, void *udata);

const char* logLevelStr(int level);
void logSetLogLevel(int level);
void logSetLock(logLockFn fn, void *udata);
void logSetQuiet(bool enable);
int logAddCallback(logLogFn fn, void *udata, int level);


#endif