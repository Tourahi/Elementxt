#include <stdio.h>
#include "cachedRender.h"


#define CELLS_X 80
#define CELLS_Y 50
#define CELL_SIZE 96
#define COMMAND_BUF_SIZE (1024 * 512)


enum { FREE_FONT, SET_CLIP, DRAW_TEXT, DRAW_RECT };

typedef struct {
  int type, size;
  RRect rect;
  RColor color;
  RFont *font;
  int tabWidth;
  char text[0];
} Command;


static unsigned cellsBuf1[CELLS_X * CELLS_Y];
static unsigned cellsBuf2[CELLS_X * CELLS_Y];
static unsigned *cellsPrev = cellsBuf1;
static unsigned *cells = cellsBuf2;
static RRect rectBuf[CELLS_X * CELLS_Y / 2];
static char commandBuf[COMMAND_BUF_SIZE];
static int commandBufIdx;
static RRect screenRect;
static bool showDebug;

static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }


/* 32bit fnv-1a hash */
#define HASH_INITIAL 2166136261

static void hash(unsigned *h, const void *data, int size) {
  const unsigned char *p = data;
  while (size--) {
    *h = (*h ^ *p++) * 16777619;
  }
}

static inline int cellIdx(int x, int y) {
  return x + y * CELLS_X;
}


static inline bool rectsOverlap(RRect a, RRect b) {
  return b.x + b.width  >= a.x && b.x <= a.x + a.width
    && b.y + b.height >= a.y && b.y <= a.y + a.height;
}


static RRect intersectRects(RRect a, RRect b) {
  int x1 = max(a.x, b.x);
  int y1 = max(a.y, b.y);
  int x2 = min(a.x + a.width, b.x + b.width);
  int y2 = min(a.y + a.height, b.y + b.height);
  return (RRect) { x1, y1, max(0, x2 - x1), max(0, y2 - y1) };
}


static RRect mergeRects(RRect a, RRect b) {
  int x1 = min(a.x, b.x);
  int y1 = min(a.y, b.y);
  int x2 = max(a.x + a.width, b.x + b.width);
  int y2 = max(a.y + a.height, b.y + b.height);
  return (RRect) { x1, y1, x2 - x1, y2 - y1 };
}


static Command* pushCommand(int type, int size) {
  Command *cmd = (*Command) (commandBuf + commandBufIdx);
  int n = commandBufIdx + size;
  if (n > COMMAND_BUF_SIZE) {
    /* TODO: WARNING SYS */
    return NULL;
  }

  commandBufIdx = n;
  memset(cmd, 0, sizeof(Command));
  cmd->type = type;
  cmd->size = size;
  return cmd;
}


static bool nextCmd(Command **prev) {
  if (*prev == NULL) {
    *prev = (Command*) commandBuf;
  } else {
    *prev = (Command*) (((char*) *prev) + (*prev)->size);
  }
  return *prev != ((Command*) (command_buf + command_buf_idx));
}

