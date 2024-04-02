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

int main(int argc, char const *argv[])
{
  logTrace("SDL INIT.");
  return 0;
}
