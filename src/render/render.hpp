#pragma once

#include "rcompiler.hpp"
#include <SDL2/SDL.h>
#include <cstdint>


class RPoint
{

public:
	R_DECL_CONSTEXPR RPoint();
	R_DECL_CONSTEXPR RPoint(int xpos, ypos);

	R_DECL_CONSTEXPR inline bool isNull() const;

	R_DECL_CONSTEXPR inline int x() const;
	R_DECL_CONSTEXPR inline int y() const;
	inline void setX(int);
	inline void setY(int);

	R_DECL_CONSTEXPR RPoint transposed() const noexcept { return {yp, xp}; }

	// returns ref to members
	R_DECL_CONSTEXPR inline int &xr();
	R_DECL_CONSTEXPR inline int &yr();

	inline RPoint &operator+=(const RPoint&);
	inline RPoint &operator-=(const RPoint&);

	inline RPoint &operator*=(float);
	inline RPoint &operator*=(double);
	inline RPoint &operator*=(int);

	inline RPoint &operator/=(double);


	R_DECL_CONSTEXPR static inline int dotProd(const RPoint &p1, const RPoint &p2)
	{ return p1.xp * p2.xp + p1.yp * p2.yp; }


private:
	int xp;
	int yp;
	int width;
	int height;
};