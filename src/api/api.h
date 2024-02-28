#ifndef API_H
#define API_H

#include "lib/lua52/lua.h"
#include "lib/lua52/lauxlib.h"
#include "lib/lua52/lualib.h"

#define API_TYPE_FONT "Font"

void apiLoadLibs(lua_State *L);

#endif