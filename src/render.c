#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "lib/stb/stb_truetype.h"
#include "render.h"

#define MAX_GLYPHSET 256

#define rectDrawLoop(expr)        \
  for (int j = y1; j < y2; j++) {   \
    for (int i = x1; i < x2; i++) { \
      *d = expr;                    \
      d++;                          \
    }                               \
    d += dr;                        \
  }

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

static inline RColor blendPixel(RColor dst, RColor src) {
  int ia = 0xff - src.a;
  dst.r = ((src.r * src.a) + (dst.r * ia)) >> 8;
  dst.g = ((src.g * src.a) + (dst.g * ia)) >> 8;
  dst.b = ((src.b * src.a) + (dst.b * ia)) >> 8;
  return dst;
}

static inline RColor blendPixel2(RColor dst, RColor src, RColor color) {
  src.a = (src.a * color.a) >> 8;
  int ia = 0xff - src.a;
  dst.r = ((src.r * color.r * src.a) >> 16) + ((dst.r * ia) >> 8);
  dst.g = ((src.g * color.g * src.a) >> 16) + ((dst.g * ia) >> 8);
  dst.b = ((src.b * color.b * src.a) >> 16) + ((dst.b * ia) >> 8);
  return dst;
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


RFont* loadFont(const char* filename, float size) {
  RFont *font = NULL;
  FILE *filep = NULL;

  /* Init Font */
  font = checkAlloc(calloc(1, sizeof(RFont)));
  font->size = size;

  /* load font into buffer */
  filep = fopen(filename, "rb");
  if (!filep) { return NULL; }

  /* get buffer size */
  fseek(filep, 0, SEEK_END); int bufferSize = ftell(filep); seek(filep, 0, SEEK_SET);

  /* load font data*/
  font->data = checkAlloc(malloc(bufferSize));
  int _ = fread(font->data, 1, bufferSize, filep); (void) _;
  fclose(filep);
  filep = NULL;

  /* init stbfont */
  int ok = stbtt_InitFont(&font->stbfont, font->data, 0);
  if (!ok) {goto failedStbfontInit;}

  /* get height and scale */
  int asc, desc, linegap;
  stbtt_GetFontVMetrics(&font->stbfont, &asc, &desc, &linegap);
  float scale = stbtt_ScaleForMappingEmToPixels(&font->stbfont, size);
  font->height = (asc - desc + linegap) * scale + 0.5;

  /* make tab and newline glyphs invisible */
  stbtt_bakedchar *g = getGlyphset(font, '\n')->glyphs;
  g['\t'].x1 = g['\t'].x0;
  g['\n'].x1 = g['\n'].x0;

  return font;

failedStbfontInit:
  if (filep) { fclose(filep); }
  if (font) { free(font->data); }
  free(font);
  return NULL;
}

void freeFont(RFont* font) {
  for (int i = 0; i < MAX_GLYPHSET; i++) {
    GlyphSet *set = font->sets[i];
    if (set) {
      freeImage(set->image);
      free(set);
    }
    free(font->data);
    free(font);
  }
}

void setFontTabWidth(RFont *font, int n) {
  GlyphSet *set = getGlyphset(font, '\t');
  set->glyphs['\t'].xadvance = n;
}

int getFontTabWidth(RFont *font) {
  GlyphSet *set = getGlyphset(font, '\t');
  return set->glyphs['\t'].xadvance;
}

int getFontWidth(RFont *font, const char *text) {
  int x = 0;
  const char *p = text;
  unsigned codepoint;
  while (*p) {
    p = utf8ToCodepoint(p, &codepoint);
    GlyphSet *set = getGlyphset(font, codepoint);
    stbtt_bakedchar *g = &set->glyphs[codepoint & 0xff];
    x += g->xadvance;
  }
  return x;
}

int getFontHeight(RFont *font) {
  return font->height;
}

void drawRect(RRect rect, RColor color) {
  if (color.a == 0) { return; }

  int x1 = rect.x < clip.left ? clip.left : rect.x;
  int y1 = rect.y < clip.top ? clip.top : rect.y;
  int x2 = rect.x + rect.width;
  int y2 = rect.y + rect.height;
  x2 = x2 > clip.right ? clip.right : x2;
  y2 = y2 > clip.bottom ? clip.bottom : y2;
  
  SDL_Surface *surf = SDL_GetWindowSurface(window);
  RColor *d = (RColor*) surf->pixels;
  d += x1 + y1 * surf->w;
  int dr = surf->w - (x2 - x1);

  if (color.a == 0xff) {
    rectDrawLoop(color);
  } else {
    rectDrawLoop(blendPixel(*d, color));
  }
}

void drawImage(RImage *image, RRect *sub, int x, int y, RColor color) {
  if (color.a == 0) { return; }

  /* clip */
  int n;
  if ((n = clip.left - x) > 0) { sub->width  -= n; sub->x += n; x += n; }
  if ((n = clip.top  - y) > 0) { sub->height -= n; sub->y += n; y += n; }
  if ((n = x + sub->width  - clip.right ) > 0) { sub->width  -= n; }
  if ((n = y + sub->height - clip.bottom) > 0) { sub->height -= n; }

  if (sub->width <= 0 || sub->height <= 0) { return; }

  /* draw */
  SDL_Surface *surf = SDL_GetWindowSurface(window);
  RColor *s = image->pixels;
  RColor *d = (RColor*) surf->pixels;
  s += sub->x + sub->y * image->width;
  d += x + y * surf->w;
  int sr = image->width - sub->width;
  int dr = surf->w - sub->width;

  for (int j = 0; j < sub->height; j++) {
    for (int i = 0; i < sub->width; i++) {
      *d = blendPixel2(*d, *s, color);
      d++;
      s++;
    }
    d += dr;
    s += sr;
  }
}


void drawText(RFont *font, const char *text, int x, int y, RColor color) {
  RRect rect;
  const char *p = text;
  unsigned codepoint;
  while (*p) {
    p = utf8ToCodepoint(p, &codepoint);
    GlyphSet *set = getGlyphset(font, codepoint);
    stbtt_bakedchar *g = &set->glyphs[codepoint & 0xff];
    rect.x = g->x0;
    rect.y = g->y0;
    rect.width = g->x1 - g->x0;
    rect.height = g->y1 - g->y0;
    drawImage(set->image, &rect, x + g->xoff, y + g->yoff, color);
    x += g->xadvance;
  }
  return x;
}