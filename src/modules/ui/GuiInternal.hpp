#pragma once

#ifndef GUI_ASSERT
#include <cassert>
#define GUI_ASSERT(exp) assert(exp)
#endif



GUI_LIB void guiZero(void *ptr, guiSize size);