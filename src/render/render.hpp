#pragma once

#include "rcompiler.hpp"
#include <SDL2/SDL.h>
#include <cstdint>



typedef struct { uint8_t r, g, b, a; } RColor;

class RRect
{

public:
	R_DECL_CONSTEXPR RRect();
	R_DECL_CONSTEXPR RRect(int xpos, int ypos, int w, int h);

	R_DECL_CONSTEXPR inline bool isNull() const;

	R_DECL_CONSTEXPR inline int x() const;
	R_DECL_CONSTEXPR inline int y() const;

	R_DECL_CONSTEXPR inline int width() const;
	R_DECL_CONSTEXPR inline int height() const;


	inline void setX(int);
	inline void setY(int);
	inline void setWidth(int);
	inline void setHeight(int);

	R_DECL_CONSTEXPR RRect transposed() const noexcept { return {yp, xp, rwidth, rheight}; }

	// returns ref to members
	inline int &xr();
	inline int &yr();
	inline int &widthr();
	inline int &heightr();

	inline RRect &operator+=(const RRect&);
	inline RRect &operator-=(const RRect&);

	R_DECL_CONSTEXPR static inline int dotProd(const RRect &p1, const RRect &p2)
	{ return p1.xp * p2.xp + p1.yp * p2.yp; }


private:
	int xp;
	int yp;
	int rwidth;
	int rheight;
};



/* ==============================================================
 *                   RRect inline functions 
 * ===============================================================*/

R_DECL_CONSTEXPR inline RRect::RRect() : xp(0), yp(0), rwidth(0), rheight(0) {}
R_DECL_CONSTEXPR inline RRect::RRect(int xpos, int ypos, int w, int h) : xp(xpos), yp(ypos), rwidth(w), rheight(h) {}

R_DECL_CONSTEXPR inline bool RRect::isNull() const 
{ return xp == 0 && yp == 0 && rwidth == 0 && rheight == 0; }

R_DECL_CONSTEXPR inline int RRect::x() const
{ return xp; }

R_DECL_CONSTEXPR inline int RRect::y() const
{ return yp; }

R_DECL_CONSTEXPR inline int RRect::width() const
{ return rwidth; }

R_DECL_CONSTEXPR inline int RRect::height() const
{ return rheight; }


inline void RRect::setX(int x)
{ xp = x; }

inline void RRect::setY(int y)
{ yp = y; }

inline void RRect::setWidth(int w)
{ rwidth = w; }

inline void RRect::setHeight(int h)
{ rheight = h; }


inline int &RRect::xr()
{ return xp; }

inline int &RRect::yr()
{ return yp; }


inline int &RRect::widthr()
{ return rwidth; }

inline int &RRect::heightr()
{ return rheight; }


inline RRect &RRect::operator+=(const RRect &p)
{ xp+=p.xp; yp+=p.yp; return *this; }

inline RRect &RRect::operator-=(const RRect &p)
{ xp-=p.xp; yp-=p.yp; return *this; }


/* ==============================================================
 *                   			Image - Font (Def: render.cpp)
 * ===============================================================*/
typedef struct RImage;
typedef struct RFont;

/* ==============================================================
 *                   			Render Functions (Def: render.cpp)
 * ===============================================================*/

void 	renderSetClipRect(RRect rect);
void 	renderInitSDLWindow(SDL_Window *win);
void 	renderGetSize(int* w, int* h);

void renderNewImage(int width, int height);

void renderSetFontTabWidth(RFont* font, int n);




// Spec stuff (will be removed in the release versions)
RRect renderGetClipDims();

