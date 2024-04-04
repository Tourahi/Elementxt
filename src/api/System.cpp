#include <SDL2/SDL.h>
#include <cstdbool>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <cerrno>
#include <cerrno>
#include <sys/stat.h>

#include "Api.hpp"
#include "CachedRenderer.hpp"
#include "tools/log.h"

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


static const char* button_name(int button) {
  switch (button) {
    case 1  : return "left";
    case 2  : return "middle";
    case 3  : return "right";
    default : return "?";
  }
}

static int fWaitEvent(lua_State *L) {
  double n = luaL_checknumber(L, 1);
  lua_pushboolean(L, SDL_WaitEventTimeout(NULL, n * 1000));
  return 1;
}

static char* keyName(char *dst, int sym) {
  strcpy(dst, SDL_GetKeyName(sym));
  char *p = dst;
  while(*p) {
    *p = tolower(*p);
    p++;
  }
  return dst;
}

static int fPollEvent(lua_State *L) {
  char buf[16];
  int mx, my, wx, wy;
  SDL_Event e;
  bool eventQ = true;

  while(eventQ) {
    if (!SDL_PollEvent(&e)) {
      return 0;
    }

    switch(e.type) {
      case SDL_QUIT:
        lua_pushstring(L, "quit");
        return 1;

      case SDL_WINDOWEVENT:
        if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
          lua_pushstring(L, "resized");
          lua_pushnumber(L, e.window.data1);
          lua_pushnumber(L, e.window.data2);
          return 3;
        } else if (e.window.event == SDL_WINDOWEVENT_EXPOSED) {
          CashedRenderer::invalidate();
          lua_pushstring(L, "exposed");
          return 1;
        }

        if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
         SDL_FlushEvent(SDL_KEYDOWN);
        }
        break;

      case SDL_DROPFILE:
        SDL_GetGlobalMouseState(&mx, &my);
        SDL_GetWindowPosition(window, &wx, &wy);
        lua_pushstring(L, "filedropped");
        lua_pushstring(L, e.drop.file);
        lua_pushnumber(L, mx - wx);
        lua_pushnumber(L, my - wy);
        SDL_free(e.drop.file);
        return 4;

      case SDL_KEYDOWN:
        lua_pushstring(L, "keypressed");
        lua_pushstring(L, keyName(buf, e.key.keysym.sym));
        return 2;

      case SDL_KEYUP:
        lua_pushstring(L, "keyreleased");
        lua_pushstring(L, keyName(buf, e.key.keysym.sym));
        return 2;

      case SDL_TEXTINPUT:
        lua_pushstring(L, "textinput");
        lua_pushstring(L, e.text.text);
        return 2;

      case SDL_MOUSEBUTTONDOWN:
        if (e.button.button == 1) { SDL_CaptureMouse((SDL_bool) 1); }
        lua_pushstring(L, "mousepressed");
        lua_pushstring(L, button_name(e.button.button));
        lua_pushnumber(L, e.button.x);
        lua_pushnumber(L, e.button.y);
        lua_pushnumber(L, e.button.clicks);
        return 5;

      case SDL_MOUSEBUTTONUP:
        if (e.button.button == 1) { SDL_CaptureMouse((SDL_bool) 0); }
        lua_pushstring(L, "mousereleased");
        lua_pushstring(L, button_name(e.button.button));
        lua_pushnumber(L, e.button.x);
        lua_pushnumber(L, e.button.y);
        return 4;

      case SDL_MOUSEMOTION:
        lua_pushstring(L, "mousemoved");
        lua_pushnumber(L, e.motion.x);
        lua_pushnumber(L, e.motion.y);
        lua_pushnumber(L, e.motion.xrel);
        lua_pushnumber(L, e.motion.yrel);
        return 5;

      case SDL_MOUSEWHEEL:
        lua_pushstring(L, "mousewheel");
        lua_pushnumber(L, e.wheel.y);
        return 2;
      
      default:
        break;
    }

  }

  return 0;
}

static int fSetCursor(lua_State *L) {
  int opt = luaL_checkoption(L, 1, "arrow", cursorOpts);
  int n = cursorEnums[opt];
  SDL_Cursor *cursor = cursorCache[n];
  if (!cursor) {
    cursor = SDL_CreateSystemCursor((SDL_SystemCursor) n);
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
  const char* path = (const char*) luaL_checkstring(L, 1);

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

static int fGetClipboard(lua_State *L)
{
  char *text = SDL_GetClipboardText();
  if (!text) { return 0; }
  lua_pushstring(L, text);
  SDL_free(text);
  return 1;
}

static int fSetCipboard(lua_State  *L) {
  const char *text = luaL_checkstring(L, 1);
  SDL_SetClipboardText(text);
  return 0;
}

static int fGeTime(lua_State *L) {
  double n = SDL_GetPerformanceCounter() / (double) SDL_GetPerformanceFrequency();
  lua_pushnumber(L, n);
  return 1;
}

static int fSleep(lua_State *L) {
  double n = luaL_checknumber(L, 1);
  SDL_Delay(n * 1000);
  return 0;
}

static int fExec(lua_State *L) {
  size_t len;
  const char *cmd = luaL_checklstring(L, 1, &len);
  char *buf = (char *) malloc(len + 32);
  if (!buf) { luaL_error(L, "buffer allocation failed"); }
#if _WIN32
  sprintf(buf, "cmd /c \"%s\"", cmd);
  WinExec(buf, SW_HIDE);
#else
  sprintf(buf, "%s &", cmd);
  int res = system(buf);
  (void) res;
#endif
  free(buf);
  return 0;
}


static int fFuzzyMatch(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  const char *ptn = luaL_checkstring(L, 2);
  int score = 0;
  int run = 0;

  while (*str && *ptn) {
    while (*str == ' ') { str++; }
    while (*ptn == ' ') { ptn++; }
    if (tolower(*str) == tolower(*ptn)) {
      score += run * 10 - (*str != *ptn);
      run++;
      ptn++;
    } else {
      score -= 10;
      run = 0;
    }
    str++;
  }
  if (*ptn) { return 0; }

  lua_pushnumber(L, score - (int) strlen(str));
  return 1;
}

static const luaL_Reg lib[] = {
  { "setCursor",           fSetCursor          },
  { "setWindowTitle",    fSetWindowTitle    },
  { "setWindowMode",     fSetWindowMode     },
  { "windowHasFocus",    fWindowHasFocus    },
  { "showConfirmDialog", fShowConfirmDialog },
  { "chdir",               fChdir               },
  { "listDir",            fListDir            },
  { "absolutePath",       fAbsolutePath       },
  { "getFileInfo",       fGetFileInfo         },
  { "getClipboard",       fGetClipboard        },
  { "sleep",               fSleep               },
  { "exec",                fExec                },
  { "fuzzyMatch",         fFuzzyMatch         },
  { NULL, NULL }
};


int luaOpenSys(lua_State *L) {
  luaL_newlib(L, lib);
  return 1;
}