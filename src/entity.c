#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "entity.h"

extern SDL_Surface *screen;
extern SDL_Rect Camera;
extern float xOffset;
extern float yOffset;

Entity EntityList[MAXENTITIES];
Entity *Player;
int NumEnts;
int gravity = 10;

int KeyCount;
Uint8 *oldkeys; /*last frame's key presses*/
Uint8 *keys;    /*current frame's key presses*/


void InitEntityList()
{
	int i, j;
	NumEnts = 0;
	for(i = 0; i < MAXENTITIES; i++)
	{
		EntityList[i].used = 0;
		EntityList[i].layer = 10;
		EntityList[i].think = NULL;
		if(EntityList[i].sprite != NULL)FreeSprite(EntityList[i].sprite);
		EntityList[i].sprite = NULL;
		EntityList[i].owner = NULL;
	}
	memset(EntityList, 0, sizeof(Entity) * MAXENTITIES);
}

void ClearEntities()
{
	int i;
	for(i = 0; i < MAXENTITIES; i++)
	{
		if(EntityList[i].used)
			FreeEntity(&EntityList[i]);
	}
}

void ClearEntitiesExcept(Entity *skip)
{
  int i;
  for(i = 0; i < MAXENTITIES; i++)
  {
    if((EntityList[i].used) && (skip != &EntityList[i]))
    {
      FreeEntity(&EntityList[i]);
    }
  }
}

Entity *NewEntity()
{
	int i;
	if((NumEnts + 1) >= MAXENTITIES)
		return NULL;
	for(i = 0; i < MAXENTITIES; i++)
	{
		if(!EntityList[i].used)
		{
			memset(&EntityList[i], 0, sizeof(Entity));
			EntityList[i].used = 1;
			EntityList[i].layer = 1;
			EntityList[i].shown = 1;
			NumEnts++;
			return &EntityList[i];
		}
	}
	return NULL;
}

void FreeEntity(Entity *ent)
{
	int j;
	NumEnts--;
	ent->used = 0;
	ent->layer = 10;
	if(ent->sprite != NULL)FreeSprite(ent->sprite);
	memset(ent, 0, sizeof(Entity));
}

void DrawEntity(Entity *ent)
{
	DrawSprite(ent->sprite, screen, ent->sx, ent->sy, ent->frame);
}


void DrawEntities()
{
	int i,j;
	for(j = 0; j < 10; j++)
	{
		for(i = 0; i < MAXENTITIES; i++)
		{
			if((EntityList[i].used > 0) && (EntityList[i].shown > 0) && (EntityList[i].layer == j))
				DrawEntity(&EntityList[i]);
		}
	}
}

void UpdateEntities()
{
	int i;

	for(i = 0; i < MAXENTITIES; i++)
	{
		if(EntityList[i].used)
		{
			if(EntityList[i].think != NULL)
			{
				EntityList[i].think(&EntityList[i]);
			}
		}
	}
}


int Collide(SDL_Rect box1, SDL_Rect box2)
{
	if((box1.x + box1.w >= box2.x) && (box1.x <= box2.x + box2.w) && (box1.y + box1.h >= box2.y) && (box1.y <= box2.y + box2.h))
		return 1;
	return 0;
}

void CheckCollisions(Entity *self, SDL_Rect box1, SDL_Rect *collision)
{
	int i = 0;
	SDL_Rect box2;
	
	self->uCheck = 0;
	self->dCheck = 0;
	self->lCheck = 0;
	self->rCheck = 0;

	for(i = 0; i < MAXENTITIES; i++)
	{
		box2.x = EntityList[i].sx + EntityList[i].bbox.x;
		box2.y = EntityList[i].sy + EntityList[i].bbox.y;
		box2.w = EntityList[i].bbox.w;
		box2.h = EntityList[i].bbox.h;

		if(Collide(box1,box2))
		{
			if(EntityList[i].tang)		/*Collision with tangible object*/
			{
				if((abs((box1.y + box1.h) - box2.y) <= abs((box1.x + box1.w) - box2.x)) && (abs((box1.y + box1.h) - box2.y) <= abs((box2.x + box2.w) - box1.x)) && (abs((box1.y + box1.h) - box2.y) <= abs((box2.y + box2.h) - box1.y)))
				{
					self->uCheck = 1;
					collision->y = box2.y;
				}
				else if((abs((box1.x + box1.w) - box2.x) <= abs((box2.x + box2.w) - box1.x)) && (abs((box1.x + box1.w) - box2.x) <= abs((box2.y + box2.h) - box1.y)))
					 {	 
						 self->lCheck = 1;
						 collision->x = box2.x;
					 }
					 else if((abs((box2.x + box2.w) - box1.x) <= abs((box2.y + box2.h) - box1.y)))
						  {	  
							  self->rCheck = 1;
							  collision->w = box2.x + box2.w;
						  }
						  else
						  {
							  self->dCheck = 1;
							  collision->h = box2.y + box2.h;
						  }
			}
		}
	}
}


Entity *MakePlayer(int x, int y)
{
	Entity *player;
	player = NewEntity();
	if(player == NULL)return player;
	player->sprite = LoadSprite("images/player.png",60,52);
	player->think = PlayerThink;
	player->shown = 1;
	player->frame = 0;
	player->state = ST_IDLE;
	player->form = FM_NONE;
	player->sx = x;
	player->sy = y;
	player->bbox.x = 14;
	player->bbox.y = 0;
	player->bbox.w = 32;
	player->bbox.h = 52;
	player->isRight = 1;
	player->delay = 0;
	player->tang = 0;
	player->health = 1;
	Player = player;
	return player;
}

void PlayerThink(Entity *self)
{
	int i = 0;
	int uCheck2 = self->uCheck;
	SDL_Rect b1, collision;

	do
	{
		b1.x = self->sx + self->bbox.x + (self->vx * (i / 10));
		b1.y = self->sy + self->bbox.y + (self->vy * (i / 10));
		b1.w = self->bbox.w;
		b1.h = self->bbox.h;
		CheckCollisions(self, b1, &collision);
		i++;
	}
	while(i <= 10);

	if(!self->isRight && self->state != ST_DYING)self->frame -= 8;


	/*Do While Alive*/
	if(self->health > 0)
	{
		/*What to Do if Colliding*/
		if(self->uCheck)
		{
			self->vy = 0;
			self->sy = collision.y - self->bbox.h;
		}
		if(self->dCheck)
		{
			self->vy = 1;
			self->sy = collision.h;
		}
		if(self->lCheck)
		{
			self->vx = 0;
			self->sx = collision.x - (self->bbox.x + self->bbox.w);
		}
		if(self->rCheck)
		{
			self->vx = 0;
			self->sx = collision.w - self->bbox.x + 0.6;
		}

		/*Keep player on screen*/
		if(self->sx + self->bbox.x <= xOffset)self->sx = xOffset - self->bbox.x;
		if(self->sx + self->bbox.x + self->bbox.w > screen->w + xOffset)self->sx = (screen->w + xOffset) - (self->bbox.x + self->bbox.w);
		if(self->sy <= 0)self->vy = 1;

		/*Movement*/
		self->sx += self->vx;
		self->sy += self->vy;

		/*Standing Still*/
		if((self->vx == 0) && self->uCheck)
			self->state = ST_IDLE;

		if(uCheck2 && !self->uCheck)self->state = ST_JUMP;

		if(self->form == FM_NONE)
		{
			/*Gravity*/
			if(self->vy <= gravity && !self->uCheck)
				self->vy += 2;

			/*Player Inputs*/
			if((isKeyHeld(SDLK_a) && !self->rCheck) || (isKeyHeld(SDLK_d) && !self->lCheck))
			{
				if(isKeyHeld(SDLK_a))							/*Move left*/
				{
					if(self->vx > -8)self->vx -= 0.8;
					if(self->vx < 0)self->isRight = 0;
				}
				if(isKeyHeld(SDLK_d))							/*Move right*/
				{
					if(self->vx < 8)self->vx += 0.8;
					if(self->vx > 0)self->isRight = 1;
				}
				if(self->uCheck && ((self->state != ST_WALK) && (self->state != ST_RUN)))	/*Initiate walk cycle*/
				{
					self->state = ST_WALK;
					self->frame = 2;
				}
				if((abs(self->vx) >= 8) && (self->state == ST_WALK))self->state = ST_RUN;
			}
			else		/*Decelerates X Movement When Nothing's Pressed*/
			{
				if(abs(self->vx) > 0.7)
				{
					if(self->vx >= 0.7)self->vx -= 0.7;
					if(self->vx <= -0.7)self->vx += 0.7;
				}
				else self->vx = 0;
			}


			if(isKeyPressed(SDLK_SPACE))			
			{
				if(self->uCheck)					/*Jump*/
				{
					self->vy = -20;
					self->uCheck = 0;
					self->state = ST_JUMP;
				}
				else if(self->state = ST_JUMP)		/*Flap*/
				{
					self->state = ST_FLAP;
					self->delay = 2;
				}
			}
		}

		if(self->form == FM_BALLOON)
		{
			/*Gravity*/
			if(self->vy <= gravity && !self->uCheck)
				self->vy += 0.25;

			/*Player Inputs*/
			if(self->uCheck)
			{
				if((isKeyHeld(SDLK_a) && !self->rCheck) || (isKeyHeld(SDLK_d) && !self->lCheck))
				{
					if(isKeyHeld(SDLK_a))							/*Move left*/
					{
						if(self->vx > -8)self->vx -= 0.4;
						if(self->vx < 0)self->isRight = 0;
					}
					if(isKeyHeld(SDLK_d))							/*Move right*/
					{
						if(self->vx < 8)self->vx += 0.4;
						if(self->vx > 0)self->isRight = 1;
					}
					if(self->uCheck && ((self->state != ST_WALK) && (self->state != ST_RUN)))	/*Initiate walk cycle*/
					{
						self->state = ST_WALK;
						self->frame = 2;
					}
					if((abs(self->vx) >= 8) && (self->state == ST_WALK))self->state = ST_RUN;
				}
				else		/*Decelerates X Movement When Nothing's Pressed*/
				{
					if(abs(self->vx) > 0.5)
					{
						if(self->vx >= 0.5)self->vx -= 0.5;
						if(self->vx <= -0.5)self->vx += 0.5;
					}
					else self->vx = 0;
				}
			}
			else
			{
				if(isKeyHeld(SDLK_a) || isKeyHeld(SDLK_d))
				{
					if(isKeyHeld(SDLK_a))							/*Move left*/
					{
						self->isRight = 0;
					}
					if(isKeyHeld(SDLK_d))							/*Move right*/
					{
						self->isRight = 1;
					}
				}
			}

			if(isKeyPressed(SDLK_SPACE))			
			{
				if(self->uCheck)					/*Jump*/
				{
					self->vy = -7;
					self->vx = int(self->vx) - (int(self->vx) % 2);
					self->uCheck = 0;
					self->state = ST_JUMP;
				}
				else if(self->state = ST_JUMP)		/*Flap*/
				{
					self->vy = -5;
					if(isKeyHeld(SDLK_a))self->vx -= 2;
					if(isKeyHeld(SDLK_d))self->vx += 2;
					self->state = ST_FLAP;
					self->delay = 4;
				}
			}
		}
	}


	/*Animations*/
	switch(self->state)
	{
		case ST_IDLE:
			self->frame = 0;
			break;
		case ST_RUN:
		case ST_WALK:
			if(abs(self->delay) == 1)
			{
				if((self->delay == 1) || (self->frame == 1))
				{
					self->frame++;
					self->delay = 6;
				}
				if((self->delay == -1) || (self->frame == 4))
				{
					if(self->frame == 4)self->frame -= 2;
					else self->frame--;
					self->delay = -6;
				}
			}
			else if(self->delay > 0)self->delay--;
				 else self->delay++;
			if((isKeyHeld(SDLK_a) && self->vx > 0) || (isKeyHeld(SDLK_d) && self->vx < 0))
			{
				self->state = ST_SKID;
				self->frame = 4;
			}
			break;
		case ST_SKID:
			if(((isKeyHeld(SDLK_a) && self->vx < 0) || (isKeyHeld(SDLK_d) && self->vx > 0)) && self->state == ST_SKID)
			{
				self->state = ST_WALK;
				self->frame = 2;
			}
			break;
		case ST_JUMP:
			self->frame = 5;
			break;
		case ST_FLAP:
			if(abs(self->delay) == 1)
			{
				if((self->delay == 1) || (self->frame == 5))
				{
					self->frame++;
					self->delay = 2;
				}
				if((self->delay == -1) || (self->frame == 8))
				{
					if(self->frame == 8)self->frame -= 2;
					else self->state = ST_JUMP;
					self->delay = -2;
				}
			}
			else if(self->delay > 0)self->delay--;
				 else self->delay++;
			break;
	}


	if(!self->isRight && self->state != ST_DYING)self->frame += 8;
}

Entity *MakeBalloon()
{
	Entity *balloon;
	balloon = NewEntity();
	if(balloon == NULL)return balloon;
	balloon->sprite = LoadSprite("images/balloons.png", 84, 60);
	balloon->think = BalloonThink;
	balloon->shown = 1;
	balloon->frame = 16;
	balloon->sx = Player->sx - 12;
	balloon->sy = Player->sy - 48;
	balloon->bbox.x = 22;
	balloon->bbox.y = 6;
	balloon->bbox.w = 36;
	balloon->bbox.h = 36;
	balloon->delay = 16;
	balloon->tang = 0;
	balloon->owner = Player;
	return balloon;
}

void BalloonThink(Entity *self)
{
	self->frame -= 16;
	if(!Player->isRight)self->frame -= 8;
	
	self->sx = Player->sx -12;
	self->sy = Player->sy - 48;


	/*Animations*/
	switch(Player->state)
	{
		case ST_IDLE:
			if(!self->delay)
			{
				switch(self->frame)
				{
					case 2:
					case 1:
						self->frame--;
						break;
					default:
						self->frame = 2;
						break;
				}
				self->delay = 16;
			}
			else self->delay--;
			break;
		case ST_RUN:
			self->frame = 3;
			self->delay = 0;
			break;
		case ST_SKID:
			self->frame = 4;
			self->delay = 0;
			break;
		case ST_JUMP:
			if(!self->delay)
			{
				switch(self->frame)
				{
					case 5:
					case 6:
						self->frame++;
						break;
					default:
						self->frame = 5;
						break;
				}
				self->delay = 16;
			}
			else self->delay--;
			break;
		case ST_FLAP:
			self->frame = 5;
			self->delay = 0;
			break;
	}

	self->frame += 16;
	if(!Player->isRight)self->frame += 8;
}

Entity *BuildBrick(int x, int y)
{
	Entity *brick;
	brick = NewEntity();
	if(brick == NULL)return brick;
	brick->sprite = LoadSprite("images/brick.png", 32, 32);
	brick->think = ObjectThink;
	brick->shown = 1;
	brick->sx = x;
	brick->sy = y;
	brick->bbox.x = 0;
	brick->bbox.y = 0;
	brick->bbox.w = 32;
	brick->bbox.h = 32;
	brick->tang = 1;
	return brick;
}

Entity *BuildColumn(int x, int y)
{
	Entity *column;
	column = NewEntity();
	if(column == NULL)return column;
	column->sprite = LoadSprite("images/column.png", 64, 64);
	column->think = ObjectThink;
	column->shown = 1;
	column->sx = x;
	column->sy = y;
	column->bbox.x = 0;
	column->bbox.y = 0;
	column->bbox.w = 64;
	column->bbox.h = 64;
	column->tang = 1;
	return column;
}

void ObjectThink(Entity *self)
{
	if(self->sx + self->bbox.w < xOffset)FreeEntity(self);
}

void BuildRoad(int x, int y, int j)
{
	for(int i = 0; i < j; i++)
	{
		BuildBrick(x + (32 * i), y);
	}
}


void InitKeyboard()
{
  keys = SDL_GetKeyState(&KeyCount);
  oldkeys = (Uint8 *)malloc(sizeof(Uint8)*KeyCount);
  if(oldkeys == NULL)
  {
    fprintf(stderr,"unable to initialize keyboard structure!\n");
  }
}

void ClearKeyboard()
{
  if(oldkeys == NULL)return;
  memset(oldkeys,0,sizeof(Uint8)*KeyCount);
}

void UpdateKeyboard()
{
  int i;
  if((oldkeys == NULL)||(keys == NULL))
  {
    return;
  }
  for(i = 0; i < KeyCount;i++)
  {
    oldkeys[i] = keys[i];
  }
  keys = SDL_GetKeyState(NULL);
}

int isKeyPressed(int key)
{
  if((oldkeys == NULL)||(keys == NULL))
  {
    return 0;
  }
  if((keys[key]) && (!oldkeys[key]))
  {
    return 1;
  }
  return 0;
}

int isKeyReleased(int key)
{
  if((oldkeys == NULL)||(keys == NULL))
  {
    return 0;
  }
  if((!keys[key]) && (oldkeys[key]))
  {
    return 1;
  }
  return 0;
}

int isKeyHeld(int key)
{
  if((oldkeys == NULL)||(keys == NULL))
  {
    return 0;
  }
  if((keys[key]) && (oldkeys[key]))
  {
    return 1;
  }
  return 0;
}