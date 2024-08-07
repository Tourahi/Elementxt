#include "../../Build.hpp"
#include <SDL2/SDL_video.h>

#ifdef RUN_TESTS

#include <iostream>

#include <SDL2/SDL.h>
#include "../../tools/utest.h"
#include "../../tools/log.h"
#include "../render.hpp"


// Structs

struct RImage {
	RColor *pixels;
	int width, height;
};

/*typedef struct {
  RImage *image;
  stbtt_bakedchar glyphs[256];
} GlyphSet;


struct RFont {
  void *data;
  stbtt_fontinfo stbfont;
  GlyphSet *sets[MAX_GLYPHSET];
  float size;
  int height;
};
*/

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


//The window we'll be rendering to
SDL_Window* gWindow = NULL;
	

static bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "Render_spec", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Init render
			renderInitSDLWindow(gWindow);
		}
	}

	return success;
}


static void close()
{
	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}


static void keepWindow()
{
#ifdef SPEC_KEEP_SDL_WIN
	// Hack 
  SDL_Event e; bool quit = false; while( quit == false ){ while( SDL_PollEvent( &e ) ){ if( e.type == SDL_QUIT ) quit = true; } }
#endif
}


UTEST(Render, InitSdlWin) {
	bool initStat = init();

	RRect clipRect = renderGetClipDims();

	ASSERT_EQ(initStat, true);
	ASSERT_EQ(clipRect.width(), 640);
	ASSERT_EQ(clipRect.height(), 480);

	SDL_UpdateWindowSurface(gWindow);

	keepWindow();

	close();
}


UTEST(Render, renderNewImage) {
	RImage *image = renderNewImage(200, 400);

	ASSERT_EQ(image->width, 200);
	ASSERT_EQ(image->height, 400);
}


#endif
