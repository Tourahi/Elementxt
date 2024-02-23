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


#endif