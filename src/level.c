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
			BuildColumn(200, 600-64);
			//BuildMovingPlatform(300, 330, 300, 530);
			//BuildMovingPlatform(700, 530, 500, 530);
			//BuildMovingPlatform(400, 530, 541, 388);

			MakeRocket();

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
