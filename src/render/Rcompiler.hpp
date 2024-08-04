#pragma once

#define R_DECL_CONSTEXPR constexpr

#ifndef RENDER_ASSERT
#include <cassert>
#define RENDER_ASSERT(exp) assert(exp)
#endif
