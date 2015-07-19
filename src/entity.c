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

void CheckCollisions(Entity *self, SDL_Rect box1, SDL_Rect *collision, int ct)
{
	int i = 0;
	SDL_Rect box2;
	
	self->uCheck = 0;
	self->dCheck = 0;
	self->lCheck = 0;
	self->rCheck = 0;
	self->above = self;

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
				if((abs((box1.y + box1.h) - box2.y) <= 16) && (abs((box1.y + box1.h) - box2.y) <= abs((box1.x + box1.w) - box2.x)) && (abs((box1.y + box1.h) - box2.y) <= abs((box2.x + box2.w) - box1.x)) && (abs((box1.y + box1.h) - box2.y) <= abs((box2.y + box2.h) - box1.y)) && (self->vy >= EntityList[i].vy) && EntityList[i].uCheck)
				{
					self->uCheck = 1;
					self->below = &EntityList[i];
					collision->y = box2.y;
					ct = 11;
				}
				else if((abs((box1.x + box1.w) - box2.x) <= 16) && (abs((box1.x + box1.w) - box2.x) <= abs((box2.x + box2.w) - box1.x)) && (abs((box1.x + box1.w) - box2.x) <= abs((box2.y + box2.h) - box1.y)) && (self->vx >= EntityList[i].vx) && EntityList[i].lCheck)
					 {	 
						 self->lCheck = 1;
						 collision->x = box2.x;
						 ct = 11;
					 }
					 else if((abs((box2.x + box2.w) - box1.x) <= 16) && (abs((box2.x + box2.w) - box1.x) <= abs((box2.y + box2.h) - box1.y)) && (self->vx <= EntityList[i].vx) && EntityList[i].rCheck)
						  {	  
							  self->rCheck = 1;
							  collision->w = box2.x + box2.w;
							  ct = 11;
						  }
						  else if((abs((box2.y + box2.h) - box1.y) <= 16) && (self->vy <= EntityList[i].vy))
							   {
								   self->above = &EntityList[i];
								   if(EntityList[i].dCheck)
								   {
									   self->dCheck = 1;
									   collision->h = box2.y + box2.h;
								   }
								   ct = 11;
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
	player->layer = 2;
	player->frame = 0;
	player->state = ST_IDLE;
	player->form = FM_NONE;
	player->sx = x;
	player->sy = y;
	player->bbox.x = 14;
	player->bbox.y = 0;
	player->bbox.w = 32;
	player->bbox.h = 52;
	player->below = player;
	player->above = player;
	player->isRight = 1;
	player->delay = 0;
	player->tang = 0;
	player->health = 1;
	return player;
}

void PlayerThink(Entity *self)
{
	int i = 0;
	int uCheck2 = self->uCheck;
	SDL_Rect b1, collision, collision2;
	
	do
	{
		b1.x = self->sx + self->bbox.x + (self->vx * (i / 10));
		b1.y = self->sy + self->bbox.y + (self->vy * (i / 10));
		b1.w = self->bbox.w;
		b1.h = self->bbox.h;
		CheckCollisions(self, b1, &collision, i);
		
		if(self->owner != NULL)
		{
			b1.x = self->owner->sx + self->owner->bbox.x + (self->vx * (i / 10));
			b1.y = self->owner->sy + self->owner->bbox.y + (self->vy * (i / 10));
			b1.w = self->owner->bbox.w;
			b1.h = self->owner->bbox.h;
			CheckCollisions(self->owner, b1, &collision2, i);
		}
		
		i++;
	}
	while(i <= 10);

	if(!self->isRight && self->state != ST_DYING)self->frame -= 10;


	/*Do While Alive*/
	if(self->health > 0)
	{
		/*Keep player on screen*/
		if(self->sx + self->bbox.x <= xOffset)self->sx = xOffset - self->bbox.x;
		if(self->sx + self->bbox.x + self->bbox.w > screen->w + xOffset)self->sx = (screen->w + xOffset) - (self->bbox.x + self->bbox.w);
		if(self->sy <= 0)self->vy = 1;

		/*Movement*/
		self->sx += (self->vx + self->below->vx);
		self->sy += (self->vy + self->below->vy/2);

		/*Standing Still*/
		if((self->vx == 0) && self->uCheck && self->state != ST_PUMP)
			self->state = ST_IDLE;

		if(self->wait > 0)
		{
			self->wait--;
			self->above = self;
		}

		switch(self->form)
		{
			case FM_NONE:
				/*Gravity*/
				if(self->vy <= gravity && !self->uCheck)
					self->vy += 2;

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
				
				/*Walk off ledge*/
				if(uCheck2 && !self->uCheck)self->state = ST_JUMP;
			
				/*Player Inputs*/
				if(((isKeyHeld(SDLK_a) && !self->rCheck) || (isKeyHeld(SDLK_d) && !self->lCheck)) && (self->state != ST_PUMP))
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


				if(isKeyPressed(SDLK_SPACE) && (self->state != ST_PUMP))			
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

				if(isKeyPressed(SDLK_p) && self->uCheck)	/*Pump*/
				{
					if(self->state != ST_PUMP)
					{
						self->state = ST_PUMP;
						self->vx = 0;
						self->delay = 0;
					}
					else
					{
						self->state = ST_IDLE;
						self->ct = 0;
					}
				}

				if((self->state == ST_PUMP) && isKeyPressed(SDLK_s))
				{
					self->frame = 9;
					self->delay = 3;
					self->ct++;
					if(self->ct == 10)
					{
						self->owner = MakeBalloon();
						self->form = FM_BALLOON1;
						self->ct = 0;
					}
				}

				if((self->above->form == FM_BALLOON1 || self->above->form == FM_BALLOON2) && (self->above->owner == NULL) && (self->wait == 0))		/*Reconnect with balloons*/
				{
					self->form = self->above->form;
					self->owner = self->above;
					self->above->owner = self;
					self->vx = int(self->vx) - (int(self->vx) % 2);
					self->vy = (self->vy + self->above->vy) / 2;
				}
				break;

			case FM_BALLOON1:
				/*Gravity*/
				if(self->vy <= gravity && !self->uCheck)
					self->vy += 0.35;

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
				if(self->lCheck || self->owner->lCheck)
				{
					if(!self->uCheck && self->vx > 0)self->vx = -abs(self->vx) + 2;
					else self->vx = 0;
					if(self->lCheck)self->sx = collision.x - (self->bbox.x + self->bbox.w);
					if(self->owner->lCheck)self->sx = collision2.x - (self->owner->bbox.x + self->owner->bbox.w);
				}
				if(self->rCheck || self->owner->rCheck)
				{
					if(!self->uCheck && self->vx < 0)self->vx = abs(self->vx) - 2;
					else self->vx = 0;
					if(self->rCheck)self->sx = collision.w - self->bbox.x + 0.6;
					if(self->owner->rCheck)self->sx = collision2.w - self->owner->bbox.x + 0.6;
				}
				
				/*Walk off ledge*/
				if(uCheck2 && !self->uCheck)
				{
					self->state = ST_JUMP;
					self->vx = int(self->vx) - (int(self->vx) % 2);
				}
			
				/*Player Inputs*/
				if(self->uCheck)
				{
					if(((isKeyHeld(SDLK_a) && !self->rCheck) || (isKeyHeld(SDLK_d) && !self->lCheck)) && (self->state != ST_PUMP))
					{
						if(isKeyHeld(SDLK_a))							/*Move left*/
						{
							if(self->vx > -8)self->vx -= 0.6;
							if(self->vx < 0)self->isRight = 0;
						}
						if(isKeyHeld(SDLK_d))							/*Move right*/
						{
							if(self->vx < 8)self->vx += 0.6;
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
						if(abs(self->vx) > 0.6)
						{
							if(self->vx >= 0.6)self->vx -= 0.6;
							if(self->vx <= -0.6)self->vx += 0.6;
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

				if(isKeyPressed(SDLK_SPACE) && (self->state != ST_PUMP))			
				{
					if(self->uCheck)					/*Jump*/
					{
						self->vy = -6;
						self->vx = int(self->vx) - (int(self->vx) % 2);
						self->uCheck = 0;
						self->state = ST_JUMP;
					}
					else if(self->state = ST_JUMP)		/*Flap*/
					{
						self->vy = -4;
						if(isKeyHeld(SDLK_a) && self->vx > -6)self->vx -= 2;
						if(isKeyHeld(SDLK_d) && self->vx < 6)self->vx += 2;
						self->state = ST_FLAP;
						self->delay = 4;
					}
				}

				if(isKeyPressed(SDLK_p) && self->uCheck)	/*(Take out / Put away) pump*/
				{
					if(self->state != ST_PUMP)
					{
						self->state = ST_PUMP;
						self->delay = 0;
						self->vx = 0;
					}
					else
					{
						self->state = ST_IDLE;
						self->ct = 0;
					}
				}

				if((self->state == ST_PUMP) && isKeyPressed(SDLK_s))	/*Pump*/
				{
					self->frame = 9;
					self->delay = 3;
					self->ct++;
					if(self->ct == 10)
					{
						self->form = FM_BALLOON2;
						self->state = ST_IDLE;
						self->ct = 0;
					}
				}

				if(isKeyPressed(SDLK_o))			/*Detach*/
				{
					self->form = FM_NONE;
					self->owner->owner = NULL;
					self->owner = NULL;
					self->above = self;
					self->wait = 20;
				}
				break;

			case FM_BALLOON2:
				/*Gravity*/
				if(self->vy <= gravity && !self->uCheck)
					self->vy += 0.25;

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
					if(!self->uCheck && self->vx > 0)self->vx = -abs(self->vx) + 2;
					else self->vx = 0;
					self->sx = collision.x - (self->bbox.x + self->bbox.w);
				}
				if(self->rCheck)
				{
					if(!self->uCheck && self->vx < 0)self->vx = abs(self->vx) - 2;
					else self->vx = 0;
					self->sx = collision.w - self->bbox.x + 0.6;
				}
				
				/*Walk off ledge*/
				if(uCheck2 && !self->uCheck)
				{
					self->state = ST_JUMP;
					self->vx = int(self->vx) - (int(self->vx) % 2);
				}
			
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
						if(isKeyHeld(SDLK_a) && self->vx > -6)self->vx -= 2;
						if(isKeyHeld(SDLK_d) && self->vx < 6)self->vx += 2;
						self->state = ST_FLAP;
						self->delay = 4;
					}
				}

				if(isKeyPressed(SDLK_o))			/*Detach*/
				{
					self->form = FM_NONE;
					self->owner->owner = NULL;
					self->owner = NULL;
					self->above = self;
					self->wait = 20;
				}
				break;
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
		case ST_PUMP:
			if(!self->delay)self->frame = 8;
			else self->delay--;
			break;
	}

	if(self->sy > 800)
	{
		FreeEntity(self);
		Player = NULL;
	}

	if(!self->isRight && self->state != ST_DYING)self->frame += 10;
}

Entity *MakeBalloon()
{
	Entity *balloon;
	balloon = NewEntity();
	if(balloon == NULL)return balloon;
	balloon->sprite = LoadSprite("images/balloons.png", 84, 60);
	balloon->think = BalloonThink;
	balloon->shown = 1;
	balloon->layer = 1;
	balloon->frame = 0;
	balloon->sx = Player->sx - 12;
	balloon->sy = Player->sy - 48;
	balloon->bbox.x = 22;
	balloon->bbox.y = 6;
	balloon->bbox.w = 36;
	balloon->bbox.h = 36;
	balloon->delay = 16;
	balloon->tang = 1;
	balloon->uCheck = 1;
	balloon->lCheck = 1;
	balloon->rCheck = 1;
	balloon->dCheck = 0;
	balloon->owner = Player;
	return balloon;
}

void BalloonThink(Entity *self)
{
	if(self->owner == Player)
	{
		self->state = Player->state;
		self->form = Player->form;
		self->isRight = Player->isRight;
		self->vx = Player->vx;
		self->vy = Player->vy / 2.5;
		self->sx = Player->sx - 12;
		self->sy = Player->sy - 48;
	}
	else
	{
		self->state = ST_JUMP;
		self->vx /= 1.02;
		self->vy -= 0.07;

		self->sx += self->vx;
		self->sy += self->vy;

		if(self->sy < (yOffset - self->sprite->h))FreeEntity(self);
	}

	if(self->form == FM_BALLOON2)self->frame -= 16;
	if(!self->isRight)self->frame -= 8;

	/*Animations*/
	switch(self->state)
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
		case ST_PUMP:
			self->frame = 0;
			break;
	}

	if(self->form == FM_BALLOON2)self->frame += 16;
	if(!self->isRight)self->frame += 8;
}

Entity *BuildBrick(int x, int y)
{
	Entity *brick;
	brick = NewEntity();
	if(brick == NULL)return brick;
	brick->sprite = LoadSprite("images/brick.png", 64, 32);
	brick->think = ObjectThink;
	brick->shown = 1;
	brick->sx = x;
	brick->sy = y;
	brick->bbox.x = 0;
	brick->bbox.y = 0;
	brick->bbox.w = 64;
	brick->bbox.h = 32;
	brick->tang = 1;
	brick->uCheck = 1;
	brick->lCheck = 1;
	brick->rCheck = 1;
	brick->dCheck = 1;
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
	column->uCheck = 1;
	column->lCheck = 0;
	column->rCheck = 1;
	column->dCheck = 1;
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
		BuildBrick(x + (64 * i), y);
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
