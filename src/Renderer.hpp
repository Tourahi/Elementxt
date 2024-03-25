#pragma once

#include <SDL2/SDL.h>
#include <cstdint>


namespace Elementxt
{
  namespace Renderer
  {
    
    typedef struct RImage RImage; 
    typedef struct RFont RFont;

    typedef struct { uint8_t r, g, b, a; } RColor;
    typedef struct { int x, y, width, height; } RRect;

    struct Color
    {
      uint8_t r, g, b, a;
    };

    struct Rect
    {
      int x, y, width, height;
    };

    void    rendererSetClipRect(Rect rect);
    void    rendererInitWindow(SDL_Window *win);
    void    rendererUpdateRects(Rect *rects, int count);
    void    rendererGetSize(int* w, int* h);
    RImage* rendererNewImage(int width, int height);
    void    rendererFreeImage(RImage *image);

  }
}