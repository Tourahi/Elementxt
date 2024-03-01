#include "log.h"

#define MAX_CALLBACKS 28

static const char *levelStrings[] = {
  "TRACE",
  "DEBUG",
  "INFO",
  "WARN",
  "ERROR",
  "FATAL"
};

typedef struct {
  logLogFn fn;
  void *udata;
  int level;
} Callback;

static struct {
  void *udata;
  logLockFn lock;
  int level;
  bool quiet;
  Callback callbacks[MAX_CALLBACKS];
} LOG;

const char* logLevelStr(int level) {
  return levelStrings[level];
}


void logSetLogLevel(int level) {
  LOG.level = level;
}


void logSetLock(logLockFn fn, void *udata) {
  LOG.lock = fn;
  LOG.udata = udata;
}

void logSetQuiet(bool enable) {
  LOG.quiet = enable;
}

int logAddCallback(logLogFn fn, void *udata, int level) {
  for (int i = 0; i < MAX_CALLBACKS; i++) {
    if (!LOG.callbacks[i].fn) {
      LOG.callbacks[i].fn = fn;
      LOG.callbacks[i].udata = udata;
      LOG.callbacks[i].level = level;
      return 0;
    }
  }
  return -1;
}
