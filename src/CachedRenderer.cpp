#include <cstdio>
#include "CachedRenderer.hpp"

#include "tools/log.h"

#define CELLS_X 80
#define CELLS_Y 50
#define CELL_SIZE 96
#define COMMAND_BUF_SIZE (1024 * 512)

enum { FREE_FONT, SET_CLIP, DRAW_TEXT, DRAW_RECT };

typedef struct {
  int type, size;
  Renderer::RRect rect;
  Renderer::RColor color;
  Renderer::RFont *font;
  int tabWidth;
  char text[0];
} Command;

static unsigned cellsBuf1[CELLS_X * CELLS_Y];
static unsigned cellsBuf2[CELLS_X * CELLS_Y];
static unsigned *cellsPrev = cellsBuf1;
static unsigned *cells = cellsBuf2;
static Renderer::RRect rectBuf[CELLS_X * CELLS_Y / 2];
static char commandBuf[COMMAND_BUF_SIZE];
static int commandBufIdx;
static Renderer::RRect screenRect;
static bool _showDebug;

static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }


/* 32bit fnv-1a hash */
#define HASH_INITIAL 2166136261

static void hash(unsigned *h, const void *data, int size) {
  const unsigned char *p = (const unsigned char *) data;
  while (size--) {
    *h = (*h ^ *p++) * 16777619;
  }
}

static inline int cellIdx(int x, int y) {
  return x + y * CELLS_X;
}


static inline bool rectsOverlap(Renderer::RRect a, Renderer::RRect b) {
  return b.x + b.width  >= a.x && b.x <= a.x + a.width
    && b.y + b.height >= a.y && b.y <= a.y + a.height;
}


static Renderer::RRect intersectRects(Renderer::RRect a, Renderer::RRect b) {
  int x1 = max(a.x, b.x);
  int y1 = max(a.y, b.y);
  int x2 = min(a.x + a.width, b.x + b.width);
  int y2 = min(a.y + a.height, b.y + b.height);
  return (Renderer::RRect) { x1, y1, max(0, x2 - x1), max(0, y2 - y1) };
}


static Renderer::RRect mergeRects(Renderer::RRect a, Renderer::RRect b) {
  int x1 = min(a.x, b.x);
  int y1 = min(a.y, b.y);
  int x2 = max(a.x + a.width, b.x + b.width);
  int y2 = max(a.y + a.height, b.y + b.height);
  return (Renderer::RRect) { x1, y1, x2 - x1, y2 - y1 };
}


static Command* pushCommand(int type, int size) {
  Command *cmd = (Command*) (commandBuf + commandBufIdx);
  int n = commandBufIdx + size;
  if (n > COMMAND_BUF_SIZE) {
    logWarn("Exhausted command buffer.");
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
  return *prev != ((Command*) (commandBuf + commandBufIdx));
}

/*
  * Useful res:
    * https://en.wikipedia.org/wiki/UTF-8
    * https://stackoverflow.com/questions/27331819/whats-the-difference-between-a-character-a-code-point-a-glyph-and-a-grapheme
*/
static const char* utf8ToCodepoint(const char* p, unsigned  *dst) {
  unsigned res, n;

  switch (*p & 0xf0)  {
    case 0xf0 : res = *p & 0x07;  n = 3;  break;
    case 0xe0 :  res = *p & 0x0f;  n = 2;  break;
    case 0xd0 :
    case 0xc0 :  res = *p & 0x1f;  n = 1;  break;
    default   :  res = *p;         n = 0;  break;
  }
  while (n--) {
    res = (res << 6) | (*(++p) & 0x3f);
  }

  *dst = res;
  return p + 1;
}

void showDebug (bool enable) {
  _showDebug = enable;
}


void freeFont (Renderer::RFont *font) {
  Command *cmd = pushCommand(FREE_FONT, sizeof(Command));
  if (cmd) { cmd->font = font; }
}

void setClipRect (Renderer::RRect rect) {
  Command *cmd = pushCommand(SET_CLIP, sizeof(Command));
  if (cmd) { cmd->rect = intersectRects(rect, screenRect); }
}

void drawRect (Renderer::RRect rect, Renderer::RColor color) {
  if (!rectsOverlap(screenRect, rect)) { return; }
  Command *cmd = pushCommand(DRAW_RECT, sizeof(Command));
  if (cmd) {
    cmd->rect = rect;
    cmd->color = color;
  }
}

int  drawText(Renderer::RFont *font, const char *text, int x, int y, Renderer::RColor color) {
  Renderer::RRect rect;
  rect.x = x;
  rect.y = y;
  rect.width = Renderer::rendererGetFontWidth(font, text);
  rect.height = Renderer::rendererGetFontHeight(font);
  
  if (rectsOverlap(screenRect, rect)) {
    int sz = strlen(text) + 1;
    Command *cmd = pushCommand(DRAW_TEXT, sizeof(Command) + sz);
    if (cmd) {
      cmd->color = color;
      cmd->font = font;
      cmd->rect = rect;
      cmd->tabWidth = Renderer::rendererGetFontTabWidth(font);
    }
  }

  return x + rect.width;
}


void invalidate(void) {
  memset(cellsPrev, 0xff, sizeof(cellsBuf1));
}

void beginFrame(void) {
  int w, h;
  Renderer::rendererGetSize(&w, &h);
  if (screenRect.width != w || h != screenRect.height) {
    screenRect.width = w;
    screenRect.height = h;
    CashedRenderer::invalidate();
  }
}

static void updateOverlappingCells(Renderer::RRect r, unsigned h) {
  int x1 = r.x /  CELL_SIZE;
  int y1 = r.y /  CELL_SIZE;
  int x2 = (r.x + r.width) /  CELL_SIZE;
  int y2 = (r.y + r.height) /  CELL_SIZE;


  for (int y = y1; y <= y2; y++) {
    for (int x = x1; x <= x2; x++){
      int idx = cellIdx(x, y);
      hash(&cells[idx], &h, sizeof(h));
    }
  }
}

static void pushRect(Renderer::RRect r, int *count) {
  /* Try to merge the rect with an existing one */
  for (int i = *count - 1; i >= 0; i--) {
    Renderer::RRect *rp = &rectBuf[i];
    if (rectsOverlap(*rp, r)) {
      *rp = mergeRects(*rp, r);
      return;
    }
  }
  /* if we cant merge we insert it */
  rectBuf[(*count)++] = r;
}

void endFrame(void)  {
  /* Update cells from commands */ 
  Command *cmd = NULL;
  Renderer::RRect cr = screenRect;
  while (nextCmd(&cmd)) {
    if (cmd->type == SET_CLIP) { cr = cmd->rect; }
    Renderer::RRect r = intersectRects(cmd->rect, cr);
    if (r.width == 0 || r.height == 0) { continue; }
    unsigned h = HASH_INITIAL;
    hash(&h, cmd, cmd->size);
    updateOverlappingCells(r, h);
  }

  int rectCount = 0;
  int maxX = screenRect.width / CELL_SIZE + 1;
  int maxY = screenRect.height / CELL_SIZE + 1;

  for (int y = 0; y < maxY; y++) {
    for (int x = 0; x < maxX; x++) {
      int idx = cellIdx(x, y);
      if (cells[idx] != cellsPrev[idx]) {
        pushRect((Renderer::RRect) { x, y, 1, 1 }, &rectCount);
      }
      cellsPrev[idx] = HASH_INITIAL;
    }
  }

  /* expand rects */
  for (int i = 0; i < rectCount; i++) {
    Renderer::RRect *r = &rectBuf[i];
    r->x *= CELL_SIZE;
    r->y *= CELL_SIZE;
    r->width *= CELL_SIZE;
    r->height *= CELL_SIZE;
    *r = intersectRects(*r, screenRect);
  }

  /* redraw updated regions */
  bool hasFreeCmds = false;
  for (int i = 0; i < rectCount; i++) {
    Renderer::RRect r = rectBuf[i];
    Renderer::rendererSetClipRect(r);

    cmd = NULL;
    while (nextCmd(&cmd)) {
      switch (cmd->type) {
        case FREE_FONT:
          hasFreeCmds = true;
          break;
        case SET_CLIP:
          Renderer::rendererSetClipRect(intersectRects(cmd->rect, r));
          break;
        case DRAW_RECT:
          Renderer::rendererDrawRect(cmd->rect, cmd->color);
          break;
        case DRAW_TEXT:
          Renderer::rendererSetFontTabWidth(cmd->font, cmd->tabWidth);
          Renderer::rendererDrawText(cmd->font, cmd->text, cmd->rect.x, cmd->rect.y, cmd->color);
          break;
      }
    }

    /* DEBUG TRUE (TODO: flag)*/
    if (true) {
      Renderer::RColor color = { (uint8_t) rand(), (uint8_t) rand(), (uint8_t) rand(), 50};
      Renderer::rendererDrawRect(r, color);
    }
  }

  /* update dirty rects */  
  if (rectCount > 0) {
    Renderer::rendererUpdateRects(rectBuf, rectCount);
  }

  if (hasFreeCmds) {
    cmd = NULL;
    while (nextCmd(&cmd)) {
      if (cmd->type == FREE_FONT) {
        Renderer::rendererFreeFont(cmd->font);
      }
    }
  }

  unsigned *tmp = cells;
  cells = cellsPrev;
  cellsPrev = tmp;
  commandBufIdx = 0;
}