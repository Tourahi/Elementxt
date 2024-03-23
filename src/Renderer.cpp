#include <cstdio>
#include <iostream>
#include <cassert>
#include <cmath>

#include "lib/stb/stb_truetype.h"

#include "Renderer.hpp"

static SDL_Window *window;

static struct
{
  int left, top, right, bottom;
} clip;

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

