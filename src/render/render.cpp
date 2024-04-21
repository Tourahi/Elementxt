#include "render.hpp"
#include "rcompiler.hpp"

static SDL_Window *window;

static struct
{
	int left, top, right, bottom;
} _clip;

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
