#include <stdio.h>
#include <SDL2/SDL.h>
#include "api/api.h"
#include "render.h"
#include <unistd.h>

SDL_Window *window;

#define SCALE 1.0



int main(int argc, char **argv)
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_EnableScreenSaver();
  SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
  atexit(SDL_Quit);

#ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR /* Available since 2.0.8 */
  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
#endif
#if SDL_VERSION_ATLEAST(2, 0, 5)
  SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#endif

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  window = SDL_CreateWindow(
    "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dm.w * 0.8,
    dm.h * 0.8, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN);

  initWindow(window);


  SDL_DestroyWindow(window);

  return 0;
}