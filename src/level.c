#include <stdlib.h>
#include "level.h"
#include "entity.h"

extern SDL_Surface *screen;
extern float xOffset;
extern float yOffset;
extern Entity *Player;

int CurrentLevel = 0;
int CurrentSection = 0;
Entity *player;

void RenderLevel(int level)
{
	if(level == 0)
	{
		if(CurrentSection == 0)
		{	
			BuildRoad(0, 600, 10);
			BuildColumn(300, 600-64);
			BuildColumn(400, 600-128);

			CurrentSection++;
		}
	}
}

void UpdateLevel()
{
	if(Player == NULL)
	{
		Player = MakePlayer(0, 500);
	}

	RenderLevel(CurrentLevel);
}
