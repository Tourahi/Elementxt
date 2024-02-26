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
echo "done"
 *  Create new Image.
 */
RImage* newImage(int width, int height);
void freeImage(RImage *image);

RFont* loadFont(const char* filename, float size);
void freeFont(RFont* font);

void setFontTabWidth(RFont *font, int n);
int getFontTabWidth(RFont *font);
int getFontWidth(RFont *font, const char *text);
int getFontHeight(RFont *font);


void drawRect(RRect rect, RColor color);
void drawImage(RImage *image, RRect *sub, int x, int y, RColor color);

int drawText(RFont *font, const char *text, int x, int y, RColor color);

#endif