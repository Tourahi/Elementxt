#include "Api.hpp"
#include "Renderer.hpp"
#include "CachedRenderer.hpp"


static int fLoad(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  float size = luaL_checknumber(L, 2);
  Renderer::RFont **self = (Renderer::RFont**) lua_newuserdata(L, sizeof(*self));
  luaL_setmetatable(L, API_TYPE_FONT);
  *self = Renderer::rendererLoadFont(filename, size);
  if (!*self) { luaL_error(L, "failed to load font"); }
  return 1;
}

static int fGc(lua_State *L) {
  Renderer::RFont **self = (Renderer::RFont**) luaL_checkudata(L, 1, API_TYPE_FONT);
  if (*self) { CashedRenderer::freeFont(*self); }
  return 0;
}

static int fSetTabWidth(lua_State *L) {
  Renderer::RFont **self = (Renderer::RFont**) luaL_checkudata(L, 1, API_TYPE_FONT);
  int n = luaL_checknumber(L, 2);
  Renderer::rendererSetFontTabWidth(*self, n);
  return 0;
}

static int fGetWidth(lua_State *L) {
  Renderer::RFont **self = (Renderer::RFont**) luaL_checkudata(L, 1, API_TYPE_FONT);
  const char *text = luaL_checkstring(L, 2);
  lua_pushnumber(L, Renderer::rendererGetFontWidth(*self, text));
  return 1;
}

static int fGetHeight(lua_State *L) {
  Renderer::RFont **self = (Renderer::RFont**) luaL_checkudata(L, 1, API_TYPE_FONT);
  lua_pushnumber(L, Renderer::rendererGetFontHeight(*self));
  return 1;
}

static const luaL_Reg lib[] = {
  { "__gc",         fGc          },
  { "load",         fLoad        },
  { "SetTabWidth",  fSetTabWidth },
  { "GetWidth",     fGetWidth    },
  { "GetHeight",    fGetHeight   },
  { NULL, NULL }
};

int luaopen_render_font(lua_State *L) {
  luaL_newmetatable(L, API_TYPE_FONT);
  luaL_setfuncs(L, lib, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  return 1;
}

