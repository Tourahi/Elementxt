#include "Renderer.hpp"

#include "../lib/stb/stb_truetype.h"
#include <SDL2/SDL_surface.h>
#include <cstdlib>
#include "../tools/log.h"


#define MAX_GLYPHSET 256
#define INIT_IMAGE_WIDTH 128
#define INIT_IMAGE_HEIGHT 128

// Structs

struct Renderer::Image {
	Renderer::Color *pixels;
	int width, height;
};

typedef struct {
  Renderer::Image *image;
  stbtt_bakedchar glyphs[256];
} GlyphSet;


struct Renderer::Font {
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

static inline Renderer::Color blendPixel(Renderer::Color dst, Renderer::Color src)
{
	int ia = 0xff - src.a;
	dst.r = ((src.r * src.a) + (dst.r * ia)) >> 8;
  dst.g = ((src.g * src.a) + (dst.g * ia)) >> 8;
  dst.b = ((src.b * src.a) + (dst.b * ia)) >> 8;
  return dst;
}


static inline Renderer::Color blendPixel2(Renderer::Color dst, Renderer::Color src, Renderer::Color color)
{
	src.a = (src.a * color.a) >> 8;
  int ia = 0xff - src.a;
  dst.r = ((src.r * color.r * src.a) >> 16) + ((dst.r * ia) >> 8);
  dst.g = ((src.g * color.g * src.a) >> 16) + ((dst.g * ia) >> 8);
  dst.b = ((src.b * color.b * src.a) >> 16) + ((dst.b * ia) >> 8);
  return dst;
}

static void* checkAlloc(void *ptr)
{
	if (!ptr) {
		logFatal("memory allocation failed");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

static GlyphSet* getGlyphset(Renderer::Font *font, int codepoint)
{

}


// End-Static



void Renderer::renderSetClipRect(Renderer::Rect rect)
{
	_clip.left 		= rect.x();
	_clip.top 		= rect.y();
	_clip.right 	= rect.x() + rect.width();
	_clip.bottom 	= rect.y() + rect.height();
}

Renderer::Rect Renderer::renderGetClipDims()
{
	return {_clip.left, _clip.top, _clip.right - _clip.left, _clip.bottom - _clip.top};
}

void Renderer::renderInitSDLWindow(SDL_Window *win)
{
	// render is not responsibe for creating the window it gets a ref of it.
	RENDER_ASSERT(win);
	SDL_Surface *s = SDL_GetWindowSurface(win);
	renderSetClipRect({0,0,s->w, s->h});
}


void Renderer::renderGetSize(int* w, int* h)
{
	SDL_Surface *s = SDL_GetWindowSurface(window);
	*w = s->w;
	*h = s->h;
}


Renderer::Image* Renderer::renderNewImage(int width, int height)
{
	assert(width > 0 && height > 0);
	Image *image = (Image*) malloc(sizeof(Image) + width * height * sizeof(Color));
	checkAlloc(image);
	image->pixels = (Color*) (image + 1);
	image->width = width;
	image->height = height;
	return image;
}

void Renderer::renderFreeImage(Renderer::Image* image)
{
	free(image);
}


static GlyphSet* loadGlyphset(Renderer::Font* font, int idx)
{
	GlyphSet *set = (GlyphSet*) checkAlloc(calloc(1, sizeof(GlyphSet)));

	// image init
	int width  = INIT_IMAGE_WIDTH;
	int height = INIT_IMAGE_HEIGHT;

	bool validBufferSize = false;


	while (!validBufferSize)
	{

		set->image = Renderer::renderNewImage(width, height);

		// basically doing this "pixels / (ascent - descent)" but fancy :).
		float s = 
			stbtt_ScaleForMappingEmToPixels(&font->stbfont, 1) /
      stbtt_ScaleForPixelHeight(&font->stbfont, 1);

    int res = stbtt_BakeFontBitmap((const unsigned char*) font->data, 0, font->size * s, 
    	(unsigned char*) set->image->pixels,
    	width, height, idx * 256, 256, set->glyphs);

    // if size is not enough DOUBLE ITTTT.
    if (res < 0) {
    	width *= 2;
    	width *= 2;
    	renderFreeImage(set->image);
    	continue;
    }
    validBufferSize = true;
	}

	int asc, desc, linegap;
  stbtt_GetFontVMetrics(&font->stbfont, &asc, &desc, &linegap);

	float scale = stbtt_ScaleForMappingEmToPixels(&font->stbfont, font->size);
	int scaledAsc = asc * scale * 0.5;

	for (int i = 0; i < 256; i++) {
    /* align glyphs properly with the baseline */
    set->glyphs[i].yoff += scaledAsc;
    /* ensure integer values for pixel-perfect rendering && to remove fractional spacing */
    set->glyphs[i].xadvance = floor(set->glyphs[i].xadvance);
  }

  /* convert 8bit data to 32bit */
  for (int i = width * height - 1; i >= 0; i--) {
    /* cast to uint8_t ptr and then offset it by i */
    uint8_t n = *((uint8_t*) set->image->pixels + i);
    set->image->pixels[i] = (Renderer::Color){ .r = 255, .g = 255, .b = 255, .a = n};
  }

  return set;
}