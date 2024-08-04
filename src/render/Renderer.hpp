#pragma once

#include "Rcompiler.hpp"
#include <SDL2/SDL.h>
#include <cstdint>


namespace Renderer
{

	typedef struct { uint8_t r, g, b, a; } Color;


	class Rect
	{

	public:
		R_DECL_CONSTEXPR Rect();
		R_DECL_CONSTEXPR Rect(int xpos, int ypos, int w, int h);

		R_DECL_CONSTEXPR inline bool isNull() const;

		R_DECL_CONSTEXPR inline int x() const;
		R_DECL_CONSTEXPR inline int y() const;

		R_DECL_CONSTEXPR inline int width() const;
		R_DECL_CONSTEXPR inline int height() const;


		inline void setX(int);
		inline void setY(int);
		inline void setWidth(int);
		inline void setHeight(int);

		R_DECL_CONSTEXPR Rect transposed() const noexcept { return {yp, xp, rwidth, rheight}; }

		// returns ref to members
		inline int &xr();
		inline int &yr();
		inline int &widthr();
		inline int &heightr();

		inline Rect &operator+=(const Rect&);
		inline Rect &operator-=(const Rect&);

		R_DECL_CONSTEXPR static inline int dotProd(const Rect &p1, const Rect &p2)
		{ return p1.xp * p2.xp + p1.yp * p2.yp; }


	private:
		int xp;
		int yp;
		int rwidth;
		int rheight;
	};



/* ==============================================================
 *                   Rect inline functions 
 * ===============================================================*/

	R_DECL_CONSTEXPR inline Rect::Rect() : xp(0), yp(0), rwidth(0), rheight(0) {}
	R_DECL_CONSTEXPR inline Rect::Rect(int xpos, int ypos, int w, int h) : xp(xpos), yp(ypos), rwidth(w), rheight(h) {}

	R_DECL_CONSTEXPR inline bool Rect::isNull() const 
	{ return xp == 0 && yp == 0 && rwidth == 0 && rheight == 0; }

	R_DECL_CONSTEXPR inline int Rect::x() const
	{ return xp; }

	R_DECL_CONSTEXPR inline int Rect::y() const
	{ return yp; }

	R_DECL_CONSTEXPR inline int Rect::width() const
	{ return rwidth; }

	R_DECL_CONSTEXPR inline int Rect::height() const
	{ return rheight; }


	inline void Rect::setX(int x)
	{ xp = x; }

	inline void Rect::setY(int y)
	{ yp = y; }

	inline void Rect::setWidth(int w)
	{ rwidth = w; }

	inline void Rect::setHeight(int h)
	{ rheight = h; }


	inline int &Rect::xr()
	{ return xp; }

	inline int &Rect::yr()
	{ return yp; }


	inline int &Rect::widthr()
	{ return rwidth; }

	inline int &Rect::heightr()
	{ return rheight; }


	inline Rect &Rect::operator+=(const Rect &p)
	{ xp+=p.xp; yp+=p.yp; return *this; }

	inline Rect &Rect::operator-=(const Rect &p)
	{ xp-=p.xp; yp-=p.yp; return *this; }




/* ==============================================================
 *                   			Image - Font (Def: render.cpp)
 * ===============================================================*/
	typedef struct Image;
	typedef struct Font;



/* ==============================================================
 *                   			Render Functions (Def: render.cpp)
 * ===============================================================*/
	void 	renderSetClipRect(Rect rect);
	void 	renderInitSDLWindow(SDL_Window *win);
	void 	renderGetSize(int* w, int* h);

	Image* renderNewImage(int width, int height);
	void renderFreeImage(Image* image);

	void renderSetFontTabWidth(Font* font, int n);

	// Spec stuff (will be removed in the release versions)
	Rect renderGetClipDims();

}


