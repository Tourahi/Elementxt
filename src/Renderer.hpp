#pragma once

#include <SDL2/SDL.h>
#include <cstdint>


namespace Elementxt
{
  namespace Renderer
  {
    struct Color
    {
      uint8_t r, g, b, a;
    };

    struct Rect
    {
      int x, y, width, height;
    };

    void rendererSetClipRect(Rect rect);
    void rendererInitWindow(SDL_Window *win);

  }
}