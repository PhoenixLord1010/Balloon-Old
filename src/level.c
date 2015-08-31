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
			BuildScreen();
			BuildBound(-32, 0, 1);
			BuildBound(-32, 720-32, 1);
			BuildBound(0,-32, 0);
			BuildBound(0, 720, 0);
			BuildBound(900, -32, 0);
			BuildBound(900, 720, 0);
			BuildBound(1800, -32, 0);
			BuildBound(1800, 720, 0);
			BuildBound(2700, -32, 0);
			BuildBound(2700, 720, 0);
			BuildBound(3600, -32, 0);
			BuildBound(3600, 720, 0);
			BuildBound(3632, 0, 1);
			BuildBound(3632, 720-32, 1);

			//BuildRoad(0, 600, 57);
			BuildColumn(200, 600-64);
			BuildColumn(800, 600-64);
			BuildMovingPlatform(300, 330, 300, 530);
			BuildMovingPlatform(700, 530, 500, 530);
			BuildMovingPlatform(400, 530, 541, 388);

			MakeRocket();

			CurrentSection++;
		}
	}
}

void UpdateLevel()
{
	if(Player == NULL)
	{
		Player = MakePlayer(200, 400);
	}

	RenderLevel(CurrentLevel);
}
