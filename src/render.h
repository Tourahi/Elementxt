#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <stdint.h>

typedef struct RImage RImage; 
typedef struct RFont RFont;

typedef struct { uint8_t r, g, b, a; } RColor;
typedef struct { int x, y, width, height; } RRect;


/**
 * Sets the clip rectangle [top-left-width-height].
 */
void renderSetClipRect(RRect rect);

/**
 * Initialize sdl window.
 */
void renderInitWindow(SDL_Window *win);

/**
 *  Updates given rects.
 */
void renderUpdateRects(RRect *rects, int count);


/**
 *  Gets size of windowSurface.
 */
void renderGetSize(int* x, int* y);


/**
echo "done"
 *  Create new Image.
 */
RImage* renderNewImage(int width, int height);
void renderFreeImage(RImage *image);

RFont* renderLoadFont(const char* filename, float size);
void renderFreeFont(RFont* font);

void renderSetFontTabWidth(RFont *font, int n);
int renderGetFontTabWidth(RFont *font);
int renderGetFontWidth(RFont *font, const char *text);
int renderGetFontHeight(RFont *font);


void renderDrawRect(RRect rect, RColor color);
void renderDrawImage(RImage *image, RRect *sub, int x, int y, RColor color);

int renderDrawText(RFont *font, const char *text, int x, int y, RColor color);

#endif