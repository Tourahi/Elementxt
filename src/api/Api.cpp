#include "Api.hpp"

int luaOpenSys(lua_State *L);
int luaOpenRenderer(lua_State *L);

static const luaL_Reg libs[] = {
  { "system",    luaOpenSys        },
  { "renderer",  luaOpenRenderer   },
  {NULL, NULL}
};

void apiLoadLibs(lua_State *L){
  for (int i = 0; libs[i].name; i++)
    luaL_requiref(L, libs[i].name, libs[i].func, 1);
}
