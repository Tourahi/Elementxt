#include "api.h"
#include "render.h"


static RColor checkColor(lua_State *L, int idx, int def) {
  RColor color;
  if (lua_isnoneornil(L, idx)) {
    return (RColor) { def, def, def, 255};
  }
  lua_rawgeti(L, idx, 1);
  lua_rawgeti(L, idx, 2);
  lua_rawgeti(L, idx, 3);
  lua_rawgeti(L, idx, 4);
  color.r = luaL_checknumber(L, -4);
  color.g = luaL_checknumber(L, -3);
  color.b = luaL_checknumber(L, -2);
  color.a = luaL_optnumber(L, -1, 255);
  lua_pop(L, 4);
  return color;
}

static int fGetSize(lua_State *L) {
  int w, h;
  renderGetSize(&w, &h);
  lua_pushnumber(L, w);
  lua_pushnumber(L, h);
  return 2;
}


static const luaL_Reg lib[] = {
  { "getSize", fGetSize },
  { NULL,      NULL     }
};


int luaopenSys(lua_State *L) {
  luaL_newlib(L, lib);
  return 1;
}




