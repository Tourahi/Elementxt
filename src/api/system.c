#include <SDL2/SDL.h>
#include <stdbool.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "api.h"
#include "cachedRender.h"
#ifdef _WIN32
  #include <windows.h>
#endif

extern SDL_Window *window;


static SDL_Cursor* cursorCache[SDL_SYSTEM_CURSOR_HAND + 1];

static const char *cursorOpts[] = {
  "arrow",
  "ibeam",
  "sizeh",
  "sizev",
  "hand",
  NULL
};

static const int cursorEnums[] = {
  SDL_SYSTEM_CURSOR_ARROW,
  SDL_SYSTEM_CURSOR_IBEAM,
  SDL_SYSTEM_CURSOR_SIZEWE,
  SDL_SYSTEM_CURSOR_SIZENS,
  SDL_SYSTEM_CURSOR_HAND
};

static int fSetCursor(lua_State *L) {
  int opt = luaL_checkoption(L, 1, "arrow", cursorOpts);
  int n = cursorEnums[opt];
  SDL_Cursor *cursor = cursorCache[n];
  if (!cursor) {
    cursor = SDL_CreateSystemCursor(n);
    cursorCache[n] = cursor;
  }
  SDL_SetCursor(cursor);
  return 0;
}

static const char *windowOpts[] = { "normal", "maximized", "fullscreen", 0 };
enum { WIN_NORMAL, WIN_MAXIMIZED, WIN_FULLSCREEN };

static int fSetWindowTitle(lua_State *L) {
  const char *title = luaL_checkstring(L, 1);
  SDL_SetWindowTitle(window, title);
  return 0;
}

static int fSetWindowMode(lua_State *L) {
  int n = luaL_checkoption(L, 1, "normal", windowOpts);
  SDL_SetWindowFullscreen(window,
    n == WIN_FULLSCREEN ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  if (n == WIN_NORMAL) { SDL_RestoreWindow(window); }
  if (n == WIN_MAXIMIZED) { SDL_MaximizeWindow(window); }
  return 0;
}

static int fWindowHasFocus(lua_State *L) {
  unsigned flags = SDL_GetWindowFlags(window);
  lua_pushboolean(L,flags & SDL_WINDOW_INPUT_FOCUS);
  return 1;
}








