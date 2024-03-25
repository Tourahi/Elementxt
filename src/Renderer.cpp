#include <cstdio>
#include <iostream>
#include <cassert>
#include <cmath>

#include "lib/stb/stb_truetype.h"

#include "Renderer.hpp"

#define MAX_GLYPHSET 256

static SDL_Window *window;

static struct
{
  int left, top, right, bottom;
} clip;


typedef struct {
  Elementxt::Renderer::RImage *image;
  stbtt_bakedchar glyphs[256];
} GlyphSet;

struct Elementxt::Renderer::RImage {
  RColor *pixels;
  int width, height;
};

struct Elementxt::Renderer::RFont {
  void *data;
  stbtt_fontinfo stbfont;
  GlyphSet *sets[MAX_GLYPHSET];
  float size;
  int height;
};



void Elementxt::Renderer::rendererSetClipRect(Elementxt::Renderer::Rect rect) {
  clip.left = rect.x;
  clip.top = rect.y;
  clip.right = rect.x + rect.width;
  clip.bottom = rect.x + rect.height;
}

void Elementxt::Renderer::rendererInitWindow(SDL_Window *win) {
  assert(win);
  window = win;
  SDL_Surface *surf = SDL_GetWindowSurface(win);
  Elementxt::Renderer::rendererSetClipRect( (Elementxt::Renderer::Rect) { 0, 0, surf->w, surf->h } );
}

void Elementxt::Renderer::rendererUpdateRects(Elementxt::Renderer::Rect *rects, int count) {
  SDL_UpdateWindowSurfaceRects(window, (SDL_Rect*) rects, count);
  static bool initFrame = true;
  if (initFrame) {
    SDL_ShowWindow(window);
    initFrame = false;
  }
}

void Elementxt::Renderer::rendererGetSize(int* w, int* h) {
  SDL_Surface *s = SDL_GetWindowSurface(window);
  *w = s->w;
  *h = s->h;
}

static void* checkAlloc(void *ptr) {
  if (!ptr) {
    fprintf(stderr, "Fatal error: memory allocation failed\n");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

Elementxt::Renderer::RImage* Elementxt::Renderer::rendererNewImage(int width, int height) {
  assert(width > 0 && height > 0);
  RImage *image = (RImage*) malloc(sizeof(RImage) + width * height * sizeof(RColor));
  checkAlloc(image);
  image->pixels = (RColor*) (image + 1);
  image->width = width;
  image->height = height;
  return image;
}

void Elementxt::Renderer::rendererFreeImage(Elementxt::Renderer::RImage *image) {
  free(image);
}

static GlyphSet* loadGlypset(Elementxt::Renderer::RFont *font, int idx) {
  GlyphSet *set = (GlyphSet*) checkAlloc(calloc(1, sizeof(GlyphSet)));

  // init image
  int width = 128;
  int height = 128;

  bool valideBufferSize = false;

  while (!valideBufferSize)
  {
    set->image = Elementxt::Renderer::rendererNewImage(width, height);

    float s = 
      stbtt_ScaleForMappingEmToPixels(&font->stbfont, 1) /
      stbtt_ScaleForPixelHeight(&font->stbfont, 1);
    
    int res = stbtt_BakeFontBitmap(
      (const unsigned char*)  font->data, 0, font->size * s, (unsigned char*) set->image->pixels,
      width, height, idx * 256, 256, set->glyphs
    );

    if (res < 0) {
      width *= 2;
      height *= 2;
      Elementxt::Renderer::rendererFreeImage(set->image);
      continue;
    }
    valideBufferSize = true;
  }
  
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
    /* cast to uint8_t ptr and then offset it by i */
    uint8_t n = *((uint8_t*) set->image->pixels + i);
    set->image->pixels[i] = (Elementxt::Renderer::RColor){ .r = 255, .g = 255, .b = 255, .a = n};
  }

  return set;
}

static GlyphSet* getGlyphset(Elementxt::Renderer::RFont *font, int codePoint) {

  int idx = (codePoint >> 8) % MAX_GLYPHSET;
  if (!font->sets[idx]) {
    font->sets[idx] = loadGlypset(font, idx);
  }
  return font->sets[idx];
}

Elementxt::Renderer::RFont* Elementxt::Renderer::rendererFreeImage(const char* filename, float size) {
  Elementxt::Renderer::RFont *font = NULL;
  FILE *filep = NULL;

  // init font
  font = (Elementxt::Renderer::RFont*) checkAlloc(calloc(1, sizeof(Elementxt::Renderer::RFont)));
  font->size = size;

  // load font onto buffer
  filep = fopen(filename, "rb");
  if (!filep) { return NULL; }

  fseek(filep, 0, SEEK_END); int bufferSize = ftell(filep); fseek(filep, 0, SEEK_SET);

  // load font data
  font->data = checkAlloc(malloc(bufferSize));
  int _ = fread(font->data, 1, bufferSize, filep); (void) _;
  fclose(filep);
  filep = NULL;

  int ok = stbtt_InitFont(&font->stbfont, (const unsigned char*)font->data, 0);
  if (!ok) {
    if (filep) { fclose(filep); }
    if (font) { free(font->data); }
    free(font);
    return NULL;
  }

  int asc, desc, linegap;
  stbtt_GetFontVMetrics(&font->stbfont, &asc, &desc, &linegap);
  float scale = stbtt_ScaleForMappingEmToPixels(&font->stbfont, size);
  font->height = (asc - desc + linegap) * scale + 0.5;

  stbtt_bakedchar *g = getGlyphset(font, '\n')->glyphs;
  g['\t'].x1 = g['\t'].x0;
  g['\n'].x1 = g['\n'].x0;

  return font;

}

