#include "render.hpp"
#include "rcompiler.hpp"

#include "../lib/stb/stb_truetype.h"


#define MAX_GLYPHSET 256

// Structs

struct RImage {
	RColor *pixels;
	int width, height;
};

typedef struct {
  RImage *image;
  stbtt_bakedchar glyphs[256];
} GlyphSet;


struct RFont {
  void *data;
  stbtt_fontinfo stbfont;
  GlyphSet *sets[MAX_GLYPHSET];
  float size;
  int height;
};

// Structs-end


// Static

static SDL_Window *window;

static struct
{
	int left, top, right, bottom;
} _clip;


static const char* utf8ToCodePoint(const char *p, unsigned *dst)
{
	unsigned res, n;

	/* 
   * inspecting the most significant bits of the first byte 
   * to determine the number of bytes used to encode the character.
  */ 
	switch (*p & 0xf0) {
		case 0xf0 : res = *p & 0x07; n = 3; break;
    case 0xe0 : res = *p & 0x0f; n = 2; break;
    case 0xd0 :
    case 0xc0 : res = *p & 0x1f; n = 1; break;
    default   : res = *p;        n = 0; break;
	}
	// loop over the remaining bytes (if any) to construct the complete code point.
	while(n--) {
		res = (res << 6) | (*(++p) & 0x3f);
	}
	*dst = res;
	return p + 1;
}

static inline RColor blendPixel(RColor dst, RColor src)
{
	int ia = 0xff - src.a;
	dst.r = ((src.r * src.a) + (dst.r * ia)) >> 8;
  dst.g = ((src.g * src.a) + (dst.g * ia)) >> 8;
  dst.b = ((src.b * src.a) + (dst.b * ia)) >> 8;
  return dst;
}


static inline RColor blendPixel2(RColor dst, RColor src, RColor color)
{
	src.a = (src.a * color.a) >> 8;
  int ia = 0xff - src.a;
  dst.r = ((src.r * color.r * src.a) >> 16) + ((dst.r * ia) >> 8);
  dst.g = ((src.g * color.g * src.a) >> 16) + ((dst.g * ia) >> 8);
  dst.b = ((src.b * color.b * src.a) >> 16) + ((dst.b * ia) >> 8);
  return dst;
}

// End-Static



void renderSetClipRect(RRect rect)
{
	_clip.left 		= rect.x();
	_clip.top 		= rect.y();
	_clip.right 	= rect.x() + rect.width();
	_clip.bottom 	= rect.y() + rect.height();
}

RRect renderGetClipDims()
{
	return {_clip.left, _clip.top, _clip.right - _clip.left, _clip.bottom - _clip.top};
}

void renderInitSDLWindow(SDL_Window *win)
{
	// render is not responsibe for creating the window it gets a ref of it.
	RENDER_ASSERT(win);
	SDL_Surface *s = SDL_GetWindowSurface(win);
	renderSetClipRect({0,0,s->w, s->h});
}
