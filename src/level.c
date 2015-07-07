#include <stdlib.h>
#include "level.h"
#include "entity.h"

extern SDL_Surface *screen;
extern float xOffset;
extern float yOffset;

int CurrentLevel = 0;
int CurrentSection = 0;
Entity *player;

void RenderLevel(int level)
{
	if(level == 0)
	{
		if(CurrentSection == 0)
		{	
			BuildRoad(0, 600, 30);
			BuildColumn(300, 600-64);

			CurrentSection++;
		}
	}
}

void UpdateLevel()
{
	if(player == NULL)
	{
		player = MakePlayer(0, 500);
		//MakeBalloon();
	}

	RenderLevel(CurrentLevel);
}
