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
			BuildBoundary(-64, 0);
			BuildBoundary(960, 0);
			
			BuildRoad(0, 600, 12);
			//BuildColumn(300, 600-64);
			//BuildColumn(450, 600-128);
			BuildMovingPlatform(300, 350, 300, 550);
			BuildMovingPlatform(500, 530, 700, 530);

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
