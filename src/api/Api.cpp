#include "Api.hpp"

int luaOpenSys(lua_State *L);

static const luaL_Reg libs[] = {
  {NULL, NULL}
};

void apiLoadLibs(lua_State *L){
  for (int i = 0; libs[i].name; i++)
    luaL_requiref(L, libs[i].name, libs[i].func, 1);
}