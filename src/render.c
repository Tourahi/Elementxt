#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "lib/stb/stb_truetype.h"
#include "render.h"

#define MAX_GLYPHSET 256

struct RImage {
  RColor *pixels;
  int width, height;
};

typedef struct {
  RImage *image;
  stbtt_bakedchar glyphs[256];
} GlyphSet;

struct RFont {
  void *data;
  stbtt_fontinfo stbfont;
  GlyphSet *sets[MAX_GLYPHSET];
  float size;
  int height;
};

static SDL_Window *window;
static struct { int left, top, right, bottom; } clip;

void setClipRect(RRect rect) {
  clip.left = rect.x;
  clip.top = rect.y;
  clip.right = rect.x + rect.width;
  clip.bottom = rect.y + rect.height;
}

void initWindow(SDL_Window *win) {
  assert(win);
  window = win;
  SDL_Surface *surf = SDL_GetWindowSurface(window);
  setClipRect( (RRect) { 0, 0, surf->w, surf->h } );
}

