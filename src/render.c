#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "lib/stb/stb_truetype.h"
#include "render.h"

#define MAX_GLYPHSET 256


/// tools

static void *checkAlloc(void *ptr) {
  if (!ptr) {
    fprintf(stderr, "Fatal error: memory allocation failed\n");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

static const char* utf8ToCodepoint(const char *p, unsigned *dst) {
  unsigned res, n;

  /* 
   * inspecting the most significant bits of the first byte 
   * to determine the number of bytes used to encode the character.
  */ 
  switch (*p & 0xf0) {
    case 0xf0 : res = *p & 0x07; n = 3; break;
    case 0xe0 : res = *p & 0x0f; n = 2; break;
    case 0xd0 :
    case 0xc0 : res = *p & 0x1f; n = 1; break;
    default   : res = *p;        n = 0; break;
  }
  // loop over the remaining bytes (if any) to construct the complete code point.
  while(n--) {
    res = (res << 6) | (*(++p) & 0x3f);
  }
  *dst = res;
  return p + 1;
}

/// End tools

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

void updateRects(RRect *rects, int count) {
  SDL_UpdateWindowSurfaceRects(window, (SDL_Rect*) rects, count);
  static bool initFrame = true;
  if (initFrame) {
    SDL_ShowWindow(window);
    initFrame = false;
  }
}

void getSize(int* x, int* y) {
  SDL_Surface *s = SDL_GetWindowSurface(window);
  *x = s->w;
  *y = s->h;
}

RImage* newImage(int width, int height) {
  assert(width > 0 && height > 0);
  RImage *image = malloc(sizeof(RImage) + width * height * sizeof(RColor));
  checkAlloc(image);
  image->pixels = (void*) (image + 1);
  image->width = width;
  image->height = height;
  return image;
}

void freeImage(RImage *image) {
  free(image);
}

static GlyphSet* loadGlyphset(RFont *font, int idx) {
  GlyphSet *set = checkAlloc(calloc(1, sizeof(GlyphSet)));

  /* init image */
  int width = 128;
  int height = 128;

retry:
  set->image = newImage(width, height);

  /* load glyphs */
  float s =
    stbtt_ScaleForMappingEmToPixels(&font->stbfont, 1) /
    stbtt_ScaleForPixelHeight(&font->stbfont, 1);
  int res = stbtt_BakeFontBitmap(
    font->data, 0, font->size * s, (void*) set->image->pixels,
    width, height, idx * 256, 256, set->glyphs);

  /* retry if buffer was not large enough */
  if (res < 0) {
    width *= 2;
    height *= 2;
    freeImage(set->image);
    goto retry;
  }
  
  /* adjust glyph yoffsets and xadvance */
  int asc, desc, linegap;
  stbtt_GetFontVMetrics(&font->stbfont, &asc, &desc, &linegap);
  float scale = stbtt_ScaleForMappingEmToPixels(&font->stbfont, font->size);
  int scaledAsc = asc * scale * 0.5;

  for (int i = 0; i < 256; i++) {
    /* align glyphs properly with the baseline */
    set->glyphs[i].yoff += scaledAsc;
    /* ensure integer values for pixel-perfect rendering && to remove fractional spacing */
    set->glyphs[i].xadvance = floor(set->glyphs[i].xadvance);
  }

  /* convert 8bit data to 32bit */
  for (int i = width * height - 1; i >= 0; i--) {
    /* cast to uint8_t ptr and then ofsset it by i */
    uint8_t n = *((uint8_t*) set->image->pixels + i);
    set->image->pixels[i] = (RColor){ .r = 255, .g = 255, .b = 255, .a = n};
  }

  return set;
}

static GlyphSet* getGlyphset(RFont *font, int codePoint) {
  /* (codePoint >> 8) basically divides by 256 */
  int idx = (codePoint >> 8) % MAX_GLYPHSET;
  if (!font->sets[idx]) {
    font->sets[idx] = loadGlyphset(font, idx);
  }
  return font->sets[idx];
}

