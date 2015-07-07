#include <stdlib.h>
#include "SDL.h"
#include "graphics.h"
#include "entity.h"
#include "level.h"

extern SDL_Surface *screen;
extern SDL_Surface *bgimage;
extern SDL_Surface *background;
extern SDL_Rect Camera;

int windowed = 1;

void Init_ALL();
void Update_ALL();
void Draw_ALL();

int main(int argc, char *argv[])
{
	int done = 0;
	int keyn;
	Uint8 *keys;
	
	Init_ALL();
	do
	{	
		ResetBuffer();
		
		keys = SDL_GetKeyState(&keyn);
		if(keys[SDLK_ESCAPE])done = 1;

		SDL_PumpEvents();

		Draw_ALL();
		Update_ALL();
	
		NextFrame();
	}while(!done);

	exit(0);
	return 0;
}

void CleanUpAll()
{
	CloseSprites();
	ClearEntities();
}

void Init_ALL()
{
	Init_Graphics(windowed);
	InitSpriteList();
	InitEntityList();
	InitKeyboard();
	atexit(CleanUpAll);
}

void Update_ALL()
{
	UpdateEntities();
	UpdateKeyboard();
	UpdateLevel();
}

void Draw_ALL()
{
	DrawEntities();
}
