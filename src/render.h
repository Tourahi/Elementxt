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
void setClipRect(RRect rect);

/**
 * Initialize sdl window.
 */
void initWindow(SDL_Window *win);

/**
 *  Updates given rects.
 */
void updateRects(RRect *rects, int count);


/**
 *  Gets size of windowSurface.
 */
void getSize(int* x, int* y);


/**
 *
 */
RImage* newImage(int width, int height);
void freeImage(RImage *image);

RFont loadFont(const char* filename, float size);
void freeFont(RFont* font);

#endif