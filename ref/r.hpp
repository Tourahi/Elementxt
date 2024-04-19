#pragma once

#include <SDL2/SDL.h>
#include <cstdint>


namespace Renderer
{
  
  typedef struct RImage RImage; 
  typedef struct RFont RFont;

  typedef struct { uint8_t r, g, b, a; } RColor;
  typedef struct { int x, y, width, height; } RRect;

  void    rendererSetClipRect(RRect rect);
  void    rendererInitWindow(SDL_Window *win);
  void    rendererUpdateRects(RRect *rects, int count);
  void    rendererGetSize(int* w, int* h);
  RImage* rendererNewImage(int width, int height);
  void    rendererFreeImage(RImage *image);
  RFont*  rendererLoadFont(const char* filename, float size);
  void    rendererFreeFont(RFont *font);
  void    rendererSetFontTabWidth(RFont *font, int n);
  int     rendererGetFontTabWidth(RFont *font);
  int     rendererGetFontWidth(RFont *font, const char *text);
  int     rendererGetFontHeight(RFont *font);

  void    rendererDrawRect(RRect rect, RColor color);
  void    rendererDrawImage(RImage *image, RRect *sub, int x, int y, RColor color);
  int     rendererDrawText(RFont *font, const char *text, int x, int y, RColor color);

}