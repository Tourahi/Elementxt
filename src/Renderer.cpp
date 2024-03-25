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


struct RFont {
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


