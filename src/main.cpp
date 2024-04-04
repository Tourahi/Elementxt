#include <cstdio>
#include <SDL2/SDL.h>

#include "Renderer.hpp"
#include "api/Api.hpp"

#include "tools/log.h"

#ifdef _WIN32
  #include <windows.h>
#elif __linux__
  #include <unistd.h>
#elif __APPLE__
  #include <mach-o/dyld.h>
#endif

SDL_Window *window;

static double getScale(void) {
  float dpi;
  SDL_GetDisplayDPI(0, NULL, &dpi, NULL);

#if _WIN32
  return dpi / 96.0;
#else
  return 1.0;
#endif
}
static void getExeFilename(char *buf, int sz) {
#if __lwpins32
  int len = GetModuleFileName(NULL, buf, sz - 1);
  buf[len] = '\0';
#elif __linux__
  char path[512];
  sprintf(path, "/proc/%d/exe", getpid());
  int len = readlink(path, buf, sz - 1);
  buf[len] = '\0';
#elif __APPLE__
  unsigned size = sz;
  _NSGetExecutablePath(buf, &size);
#else
  strcpy(buf, "./Elementxt");
#endif

}


int main(int argc, char const *argv[])
{
#ifdef _WIN32
  HINSTANCE lib = LoadLibrary("user32.dll");
  int (*SetProcessDPIAware)() = (void*) GetProcAddress(lib, "SetProcessDPIAware");
  SetProcessDPIAware();
#endif

  logTrace("SDL INIT.");

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_EnableScreenSaver();
  SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
  atexit(SDL_Quit);

#ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR 
  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
#endif
#if SDL_VERSION_ATLEAST(2, 0, 5)
  SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#endif

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  logTrace("SDL CREATE WINDOW.");

  // Todo: Some type of a setting file we read at the start that gives the user the ability to provide
  // custom SDL flags and initial dims.
  window = SDL_CreateWindow(
    "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dm.w * 0.5, dm.h * 0.5,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN
  );

  Renderer::rendererInitWindow(window); // send the window to the renderer

  logTrace("INIT LUA STATE.");

  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  apiLoadLibs(L);

  lua_newtable(L);
  for (int i = 0; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i + 1);
  }
  lua_setglobal(L, "ARGS");


  lua_pushstring(L, "0.0.0");
  lua_setglobal(L, "VERSION");

  lua_pushstring(L, SDL_GetPlatform());
  lua_setglobal(L, "PLATFORM");

  lua_pushnumber(L, getScale());
  lua_setglobal(L, "SCALE");

  char exename[2048];
  getExeFilename(exename, sizeof(exename));
  lua_pushstring(L, exename);
  lua_setglobal(L, "EXEFILE");


  (void) luaL_dostring(L,
    "local core\n"
    "xpcall(function()\n"
    "  SCALE = tonumber(os.getenv(\"LITE_SCALE\")) or SCALE\n"
    "  PATHSEP = package.config:sub(1, 1)\n"
    "  print(EXEFILE, PATHSEP)\n"
    "  EXEDIR = EXEFILE:match(\"^(.+)[/\\\\].*$\")\n"
    "  package.path = EXEDIR .. '/modules/?.lua;' .. package.path\n"
    "  package.path = EXEDIR .. '/modules/?/init.lua;' .. package.path\n"
    "  core = require('core')\n"
    "  print(core)\n"
    "end, function(err)\n"
    "  print('Error: ' .. tostring(err))\n"
    "  print(debug.traceback(nil, 2))\n"
    "  if core and core.on_error then\n"
    "    pcall(core.on_error, err)\n"
    "  end\n"
    "  os.exit(1)\n"
    "end)");

  lua_close(L);
  SDL_DestroyWindow(window);

  return EXIT_SUCCESS;
}
