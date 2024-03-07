#ifndef CACHEDREN_H
#define CACHEDREN_H


#include <stdbool.h>
#include "render.h"


void crShowDebug (bool enable);
void crFreeFont (RFont *font);
void crSetClipRect (RRect rect);
void crDrawRect (RRect rect, RColor color);

int  crDrawText(RFont *font, const char *text, int x, int y, RColor color);


#endif