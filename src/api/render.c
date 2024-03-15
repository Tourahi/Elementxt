#include "api.h"
#include "render.h"
#include "cachedRender.h"


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

static int fBeginFrame(lua_State  *L) {
  crBeginFrame();
  return 0;
}

static int fEndFrame(lua_State  *L) {
  crEndFrame();
  return 0;
}

static int fSetClipRect(lua_State  *L) {
  RRect rect;
  rect.x = luaL_checknumber(L, 1);
  rect.y = luaL_checknumber(L, 2);
  rect.width = luaL_checknumber(L, 3);
  rect.height = luaL_checknumber(L, 4);
  crSetClipRect(rect);
  return 0;
}

static int fDrawRect(lua_State  *L) {
  RRect rect;
  rect.x = luaL_checknumber(L, 1);
  rect.y = luaL_checknumber(L, 2);
  rect.width = luaL_checknumber(L, 3);
  rect.height = luaL_checknumber(L, 4);
  RColor color = checkColor(L, 5, 255);
  crDrawRect(rect, color);
  return 0;
}

static int fDrawText(lua_State  *L) {
  RFont **font = luaL_checkudata(L, 1, API_TYPE_FONT);
  const char *text = luaL_checkstring(L, 2);
  int x = luaL_checknumber(L, 3);
  int y = luaL_checknumber(L, 4);
  RColor color = checkColor(L, 5, 255);
  x = crDrawText(*font, text, x, y, color);
  lua_pushnumber(L, x);
  return 1;
}

static const luaL_Reg lib[] = {
  { "getSize", fGetSize },
  { "beginFrame", fBeginFrame },
  { "endFRame", fEndFrame },
  { "setClipRect", fSetClipRect },
  { "drawRect", fDrawRect },
  { "drawText", fDrawText },
  { NULL,      NULL     }
};

int luaopen_render_font(lua_State *L);

int luaopenSys(lua_State *L) {
  luaL_newlib(L, lib);
  // FONT TODO
  return 1;
}




