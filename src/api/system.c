#include <SDL2/SDL.h>
#include <stdbool.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "api.h"
#include "cachedRender.h"
#include "log.h"
#ifdef _WIN32
  #include <windows.h>
#endif

/// Window
extern SDL_Window *window;
static const char *windowOpts[] = { "normal", "maximized", "fullscreen", 0 };
enum { WIN_NORMAL, WIN_MAXIMIZED, WIN_FULLSCREEN };

/// Cursor
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

static int fShowConfirmDialog(lua_State *L) {
  const char *title = luaL_checkstring(L, 1);
  const char *msg = luaL_checkstring(L, 2);

#if _WIN32
  
  int id = MessageBox(0, msg, title, MB_YESNO |  MB_ICONWARNING);  
  lua_pushboolean(L, id == IDYES);

#else
  
  SDL_MessageBoxButtonData buttons[] = {
    { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
    { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "No" },
  };

  SDL_MessageBoxData data = {
    .title = title,
    .message = msg,
    .numbuttons = 2,
    .buttons = buttons,
  };
  int buttonid;
  SDL_ShowMessageBox(&data, &buttonid);
  lua_pushboolean(L, buttonid == 1);

#endif
  return 1;
}

static int fChdir(lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  int err = chdir(path);
  if (err) {
    logFatal("chdir() failed");
    luaL_error(L, "chdir() failed");
  }
  return 0;
}


static int fListDir(lua_State *L)
{
  const char *path = luaL_checkstring(L, 1);

  DIR *dir = opendir(path);
  if (!dir) {
    logFatal("opendir() failed");
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));
    return 2;
  }

  lua_newtable(L);
  int i = 1;
  struct dirent *entry;

  while ( (entry = readdir(dir)) ) {
    if (strcmp(entry->d_name, ".") == 0){ continue; }
    if (strcmp(entry->d_name, "..") == 0){ continue; }
    lua_pushstring(L, entry->d_name);
    lua_rawseti(L, -2, i);
    i++;
  }

  closedir(dir);
  return 1;
}

#ifdef _WIN32
  #include <windows.h>
  #define realpath(x, y) _fullpath(y, x, MAX_PATH)
#endif

static int fAbsolutePath(lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  char *res = realpath(path, NULL);
  if (!res) { return 0; }
  lua_pushstring(L, res);
  free(res);
  return 1;
}

static int fGetFileInfo(lua_State *L) {
  const char path = luaL_checkstring(L, 1);

  struct stat s;
  int err = stat(path, &s);
  if (err < 0) {
    logError("Failed getting file attributes");
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));
    return 2;
  }

  lua_newtable(L);
  lua_pushnumber(L, s.st_mtime);
  lua_setfield(L, -2, "modified");

  lua_pushnumber(L, s.st_mtime);
  lua_setfield(L, -2, "size");

  if (S_ISREG(s.st_mode)) {
    lua_pushstring(L, "file");
  } else if (S_ISDIR(s.st_mode)) {
    lua_pushstring(L, "dir");
  } else {
    lua_pushnil(L);
  }

   lua_setfield(L, -2, "type");

   return 1;
}









