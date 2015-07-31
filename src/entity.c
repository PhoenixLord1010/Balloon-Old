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
	self->above = NULL;
	self->below = NULL;
	self->left = NULL;
	self->right = NULL;

	for(i = 0; i < MAXENTITIES; i++)
	{
		box2.x = EntityList[i].sx + EntityList[i].bbox.x;
		box2.y = EntityList[i].sy + EntityList[i].bbox.y;
		box2.w = EntityList[i].bbox.w;
		box2.h = EntityList[i].bbox.h;

		if(Collide(box1,box2) && self->sprite != EntityList[i].sprite)
		{
			if(EntityList[i].tang)		/*Collision with tangible object*/
			{
				if((abs((box1.y + box1.h) - box2.y) <= 12) && (abs((box1.y + box1.h) - box2.y) <= abs((box1.x + box1.w) - box2.x)) && (abs((box1.y + box1.h) - box2.y) <= abs((box2.x + box2.w) - box1.x)) && (abs((box1.y + box1.h) - box2.y) <= abs((box2.y + box2.h) - box1.y)) && (self->vy + 2 >= EntityList[i].vy) && EntityList[i].uTang)
				{
					self->uCheck = 1;
					self->below = &EntityList[i];
					collision->y = box2.y;
				}
				else if((abs((box1.x + box1.w) - box2.x) <= 12) && (abs((box1.x + box1.w) - box2.x) <= abs((box2.x + box2.w) - box1.x)) && (abs((box1.x + box1.w) - box2.x) <= abs((box2.y + box2.h) - box1.y)) && (self->vx >= EntityList[i].vx) && EntityList[i].lTang)
					 {	 
						 self->lCheck = 1;
						 self->right = &EntityList[i];
						 collision->x = box2.x;
					 }
					 else if((abs((box2.x + box2.w) - box1.x) <= 12) && (abs((box2.x + box2.w) - box1.x) <= abs((box2.y + box2.h) - box1.y)) && (self->vx <= EntityList[i].vx) && EntityList[i].rTang)
						  {	  
							  self->rCheck = 1;
							  self->left = &EntityList[i];
							  collision->w = box2.x + box2.w;
						  }
						  else if((abs((box2.y + box2.h) - box1.y) <= 12) && (self->vy <= EntityList[i].vy))
							   {
								   self->above = &EntityList[i];
								   if(EntityList[i].dTang)
								   {
									   self->dCheck = 1;
									   collision->h = box2.y + box2.h;
								   }
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
	player->tang = 1;
	player->uTang = 1;
	player->dTang = 1;
	player->lTang = 1;
	player->rTang = 1;
	player->movable = 1;
	player->health = 1;
	return player;
}

void PlayerThink(Entity *self)
{
	int i = 0;
	int uCheck2 = self->uCheck;
	SDL_Rect b1, collision, collision2;

	float grav, speed, accel, decel, jump, flap;

	switch(self->form) 
	{	
		case FM_BALLOON1:
			grav = 0.35;
			speed = 7.5;
			accel = 0.6;
			decel = 0.6;
			jump = -6;
			flap = -4;
			break;
		case FM_BALLOON2:
			grav = 0.25;
			speed = 7;
			accel = 0.4;
			decel = 0.5;
			jump = -7;
			flap = -5;
			break;
		case FM_CHUTE:
			grav = 0.1;
			speed = 5;
			accel = 0.2;
			decel = 0.2;
			jump = 0.5;
			flap = self->vy;
			break;
		case FM_NONE:
		default:
			grav = 2;
			speed = 8;
			accel = 0.8;
			decel = 0.7;
			jump = -20;
			break;
	}
	
	do
	{
		b1.x = self->sx + self->bbox.x + (self->vx * (i / 10));
		if(!self->uCheck && self->vy >= 0)b1.y = self->sy + self->bbox.y + ((self->vy + grav) * (i / 10));
		else b1.y = self->sy + self->bbox.y + (self->vy * (i / 10));
		b1.w = self->bbox.w;
		b1.h = self->bbox.h;
		CheckCollisions(self, b1, &collision);
		
		if(self->owner != NULL)
		{
			b1.x = self->owner->sx + self->owner->bbox.x + (self->vx * (i / 10));
			b1.y = self->owner->sy + self->owner->bbox.y + (self->vy * (i / 10));
			b1.w = self->owner->bbox.w;
			b1.h = self->owner->bbox.h;
			CheckCollisions(self->owner, b1, &collision2);
		}
		
		i++;
	}
	while(i <= 10);

	if(!self->isRight && self->state != ST_DYING)self->frame -= 10;


	/*Do While Alive*/
	if(self->health > 0)
	{
		/*Movement*/
		self->sx += self->vx;
		self->sy += self->vy;
		if(self->below != NULL)
		{
			self->sx += self->below->vx;
			self->sy += self->below->vy;
		}
		if(self->left != NULL)
		{
			self->sx += self->left->vx;
		}
		if(self->right != NULL)
		{
			self->sx += self->right->vx;
		}
		
		/*Standing Still*/
		if((self->vx == 0) && self->uCheck && uCheck2 && self->state != ST_PUMP)
			self->state = ST_IDLE;

		if(self->wait > 0)
		{
			self->wait--;
		}
			
		/*Gravity*/
		if(self->vy <= gravity && !self->uCheck)
			self->vy += grav;

		/*Chute*/
		if(self->form == FM_CHUTE && self->vy > jump)
		{
			if(self->vy / 1.15 >= jump)
				self->vy /= 1.15;
			else
				self->vy = jump;
		}

		/*What to Do if Colliding*/
		if(self->uCheck)
		{
			self->vy = 0;
			self->sy = collision.y - self->bbox.h;
		}
		if(self->dCheck)
		{
			self->vy = 1 + self->above->vy;
			self->sy = collision.h;
		}
		if(self->lCheck)
		{
			if(!self->right->movable)
			{
				if(!self->uCheck && self->form != FM_NONE)self->vx = -abs(self->vx) + 2;
				else self->vx = 0;
			}
			else self->right->vx = self->vx / 2;
			self->sx = collision.x - (self->bbox.x + self->bbox.w);
		}
		if(self->rCheck)
		{
			if(!self->left->movable)
			{
				if(!self->uCheck && self->form != FM_NONE)self->vx = abs(self->vx) - 2;
				else self->vx = 0;
			}
			else self->left->vx = self->vx / 2;
			self->sx = collision.w - self->bbox.x + 0.6;
		}
		if(self->form == FM_BALLOON1 || self->form == FM_BALLOON2)		/*Balloon Collisions*/
		{
			if(self->owner->dCheck)
			{
				self->vy = 1 + self->owner->above->vy;
				self->sy = collision2.h + (self->owner->sprite->h - (self->owner->bbox.y + 12));
			}
			if(self->owner != NULL)
			{
				if(self->owner->lCheck)
				{
					if(!self->uCheck && (self->vx - self->owner->right->vx > 0))
					{
						self->vx = -abs(int(self->vx - self->owner->right->vx) - (int(self->vx - self->owner->right->vx) % 2)) + 2;
						self->sx = collision2.x - (self->owner->bbox.x + self->owner->bbox.w - 12);
					}
					else
					{
						if(self->uCheck && (self->vx + self->owner->right->vx != 0))
						{
							self->owner->vx = -abs(self->owner->vx);
							self->form = FM_NONE;
							self->owner->owner = NULL;
							self->owner = NULL;
							self->above = self;
							self->wait = 20;
						}
					}
				}
			}
			if(self->owner != NULL)
			{
				if(self->owner->rCheck)
				{
					if(!self->uCheck && (self->vx - self->owner->left->vx < 0))
					{
						self->vx = abs(int(self->vx - self->owner->left->vx) - (int(self->vx - self->owner->left->vx) % 2)) - 2;
						self->sx = collision2.w - self->owner->bbox.x + 12.6;
					}
					else
					{
						if(self->uCheck && (self->vx + self->owner->left->vx != 0))
						{
							self->owner->vx = abs(self->owner->vx);
							self->form = FM_NONE;
							self->owner->owner = NULL;
							self->owner = NULL;
							self->above = self;
							self->wait = 20;
						}
					}
				}
			}
		}
		if(self->form == FM_CHUTE)		/*Parachute collisions*/
		{
			if(self->owner->dCheck || self->owner->lCheck || self->owner->rCheck)
			{
				self->form = FM_NONE;
				self->owner->health = 0;
				self->owner->frame = 3;
				self->owner->delay = 12;
				self->owner = NULL;
			}
			if(self->uCheck)
			{
				FreeEntity(self->owner);
				self->form = FM_NONE;
				self->owner = NULL;
			}
		}
				
		/*Walk off ledge*/
		if(uCheck2 && !self->uCheck)
		{
			self->state = ST_JUMP;
			if(self->form != FM_NONE)self->vx = int(self->vx) - (int(self->vx) % 2);
		}
			
		/*Player Inputs*/
		if(self->uCheck || self->form == FM_NONE || self->form == FM_CHUTE)
		{
			if(((isKeyHeld(SDLK_a) && !self->rCheck) || (isKeyHeld(SDLK_d) && !self->lCheck)) && (self->state != ST_PUMP))
			{
				if(isKeyHeld(SDLK_a))							/*Move left*/
				{
					if(self->vx > -speed)self->vx -= accel;
					if(self->vx < 0)self->isRight = 0;
				}
				if(isKeyHeld(SDLK_d))							/*Move right*/
				{
					if(self->vx < speed)self->vx += accel;
					if(self->vx > 0)self->isRight = 1;
				}
				if(self->uCheck && ((self->state != ST_WALK) && (self->state != ST_RUN)))	/*Initiate walk cycle*/
				{
					self->state = ST_WALK;
					self->frame = 2;
				}
				if((abs(self->vx) >= speed) && (self->state == ST_WALK))self->state = ST_RUN;
			}
			else		/*Decelerates X Movement When Nothing's Pressed*/
			{
				if(abs(self->vx) > decel)
				{
					if(self->vx >= decel)self->vx -= decel;
					if(self->vx <= -decel)self->vx += decel;
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
				self->vy = jump;
				self->uCheck = 0;
				self->state = ST_JUMP;
				if(self->form != FM_NONE)self->vx = int(self->vx) - (int(self->vx) % 2);
			}
			else if(self->state = ST_JUMP)		/*Flap*/
			{
				if(self->form != FM_NONE)
				{
					self->vy = flap;
					if(isKeyHeld(SDLK_a) && self->vx > -6)self->vx -= 2;
					if(isKeyHeld(SDLK_d) && self->vx < 6)self->vx += 2;
				}
				self->state = ST_FLAP;
				self->delay = 4;
			}
		}

		if(isKeyPressed(SDLK_p) && self->uCheck && self->form != FM_BALLOON2)	/*(Take out / Put away) pump*/
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

		if((self->state == ST_PUMP) && isKeyPressed(SDLK_s) && !self->delay)	/*Pump*/
		{
			self->frame = 9;
			self->delay = 4;
			self->ct++;
			if(self->ct == 6)
			{
				if(self->form == FM_NONE)
				{
					self->owner = MakeBalloon();
					self->form = FM_BALLOON1;
				}
				else
				{
					self->form = FM_BALLOON2;
					self->state = ST_IDLE;
				}
				self->ct = 0;
			}
		}

				
		if(isKeyPressed(SDLK_o) && self->form != FM_NONE)	/*Detach*/
		{
			if(self->form == FM_BALLOON1 || self->form == FM_BALLOON2)
			{
				self->form = FM_NONE;
				self->owner->owner = NULL;
				self->owner = NULL;
				self->above = self;
				self->wait = 20;
			}
			if(self->form == FM_CHUTE)
			{
				self->form = FM_NONE;
				self->owner->health = 0;
				self->owner->frame = 3;
				self->owner->delay = 12;
				self->owner = NULL;
			}
		}
					
		if(self->above != NULL && self->form == FM_NONE)	/*Reconnect*/
		{
			if((self->above->form == FM_BALLOON1 || self->above->form == FM_BALLOON2) && (self->above->owner == NULL) && (self->wait == 0))		
			{
				self->form = self->above->form;
				self->owner = self->above;
				self->above->owner = self;
				self->vx = int(self->vx) - (int(self->vx) % 2);
				self->vy = (self->vy + self->above->vy) / 2;
			}
		}

		if(isKeyPressed(SDLK_p) && !self->uCheck && self->vy >= 0 && self->form == FM_NONE)		/*Pull out parachute*/
		{
			self->owner = MakeChute();
			self->form = FM_CHUTE;
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
		if(self->owner != NULL)self->owner->owner = NULL;
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
	balloon->uTang = 1;
	balloon->lTang = 1;
	balloon->rTang = 1;
	balloon->dTang = 0;
	balloon->movable = 1;
	balloon->owner = Player;
	return balloon;
}

void BalloonThink(Entity *self)
{
	int i = 0;
	SDL_Rect b1, collision;
	
	if(self->owner == Player && Player != NULL)
	{
		self->state = Player->state;
		self->form = Player->form;
		self->isRight = Player->isRight;
		self->vx = Player->vx / 1.5;
		self->vy = Player->vy / 2.5;
		self->sx = Player->sx - 12;
		self->sy = Player->sy - 48;
	}
	else
	{
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

		if(self->lCheck)
		{
			self->vx = -abs(self->vx);
			self->vx += self->right->vx;
		}
		if(self->rCheck)
		{
			self->vx = abs(self->vx);
			self->vx += self->left->vx;
		}
		
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
		case ST_PUMP:
		case ST_IDLE:
			if(self->form == FM_BALLOON1)	/*Position*/
			{
				if(self->isRight)
				{
					self->bbox.x = 22;
					self->bbox.y = 6;
					self->bbox.w = 36;
				}
				else
				{
					self->bbox.x = 26;
					self->bbox.y = 6;
					self->bbox.w = 36;
				}
			}
			else
			{
				if(self->isRight)
				{
					self->bbox.x = 10;
					self->bbox.y = 6;
					self->bbox.w = 60;
				}
				else
				{
					self->bbox.x = 14;
					self->bbox.y = 6;
					self->bbox.w = 60;
				}
			}

			if(!self->delay)		/*Animation*/
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
			if(self->form == FM_BALLOON1)	/*Position*/
			{
				if(self->isRight)
				{
					self->bbox.x = 2;
					self->bbox.y = 10;
					self->bbox.w = 36;
				}
				else
				{
					self->bbox.x = 46;
					self->bbox.y = 10;
					self->bbox.w = 36;
				}
			}
			else
			{
				if(self->isRight)
				{
					self->bbox.x = 2;
					self->bbox.y = 8;
					self->bbox.w = 52;
				}
				else
				{
					self->bbox.x = 30;
					self->bbox.y = 8;
					self->bbox.w = 52;
				}
			}

			self->frame = 3;
			self->delay = 0;
			break;
		case ST_SKID:
			if(self->form == FM_BALLOON1)	/*Position*/
			{
				if(self->isRight)
				{
					self->bbox.x = 30;
					self->bbox.y = 6;
					self->bbox.w = 36;
				}
				else
				{
					self->bbox.x = 18;
					self->bbox.y = 6;
					self->bbox.w = 36;
				}
			}
			else
			{
				if(self->isRight)
				{
					self->bbox.x = 14;
					self->bbox.y = 8;
					self->bbox.w = 52;
				}
				else
				{
					self->bbox.x = 18;
					self->bbox.y = 8;
					self->bbox.w = 52;
				}
			}

			self->frame = 4;
			self->delay = 0;
			break;
		case ST_JUMP:
			if(self->form == FM_BALLOON1)	/*Position*/
			{
				if(self->isRight)
				{
					self->bbox.x = 22;
					self->bbox.y = 2;
					self->bbox.w = 36;
				}
				else
				{
					self->bbox.x = 26;
					self->bbox.y = 2;
					self->bbox.w = 36;
				}
			}
			else
			{
				if(self->isRight)
				{
					self->bbox.x = 10;
					self->bbox.y = 2;
					self->bbox.w = 60;
				}
				else
				{
					self->bbox.x = 14;
					self->bbox.y = 2;
					self->bbox.w = 60;
				}
			}
			
			if(!self->delay)	/*Animation*/
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

	if(self->form == FM_BALLOON2)self->frame += 16;
	if(!self->isRight)self->frame += 8;
}

Entity *MakeChute()
{
	Entity *chute;
	chute = NewEntity();
	if(chute == NULL)return chute;
	chute->sprite = LoadSprite("images/parachute.png", 68, 52);
	chute->think = ChuteThink;
	chute->shown = 1;
	chute->layer = 1;
	chute->frame = 0;
	chute->sx = Player->sx - 4;
	chute->sy = Player->sy - 52;
	chute->bbox.x = 8;
	chute->bbox.y = 4;
	chute->bbox.w = 52;
	chute->bbox.h = 48;
	chute->delay = 4;
	chute->tang = 1;
	chute->uTang = 1;
	chute->lTang = 1;
	chute->rTang = 1;
	chute->dTang = 0;
	chute->movable = 0;
	chute->health = 1;
	chute->owner = Player;
	return chute;
}

void ChuteThink(Entity *self)
{
	int i = 0;
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
	
	if(Player != NULL)
	{
		self->sx = Player->sx - 4;
		self->sy = Player->sy - 52;
	}
	else FreeEntity(self);

	if(!self->delay)
	{
		if(self->frame < 2)
		{
			self->frame++;
			self->delay = 6;
		}
		if(!self->health)FreeEntity(self);
	}
	else self->delay--;
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
	brick->uTang = 1;
	brick->lTang = 1;
	brick->rTang = 1;
	brick->dTang = 1;
	brick->movable = 0;
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
	column->uTang = 1;
	column->lTang = 0;
	column->rTang = 1;
	column->dTang = 1;
	column->movable = 0;
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

Entity *BuildMovingPlatform(int x, int y, int a, int b)
{
	Entity *plat;
	plat = NewEntity();
	if(plat == NULL)return plat;
	plat->sprite = LoadSprite("images/movingplatform.png", 64, 16);
	plat->think = PlatThink;
	plat->shown = 1;
	plat->sx = x;
	plat->sy = y;
	plat->bbox.x = 0;
	plat->bbox.y = 0;
	plat->bbox.w = 64;
	plat->bbox.h = 16;
	plat->tang = 1;
	plat->uTang = 1;
	plat->dTang = 0;
	plat->lTang = 1;
	plat->rTang = 1;
	plat->movable = 0;
	plat->health = x;			/*Point 1's x position*/
	plat->delay = y;			/*Point 1's y position*/
	plat->ct = a;			/*Point 2's x position*/
	plat->wait = b;			/*Point 2's y position*/
	plat->isRight = 1;
	return plat;
}

void PlatThink(Entity *self)
{
	int i = 0;
	SDL_Rect b1, collision;
	float px, py, pz, hyp;
	float speed = 2;
	
	if(self->vy > 0)
	{
		self->sy += 2;
		self->bbox.y -= 2;
	}
	
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
	
	if(self->isRight)
	{
		px = self->ct - self->sx;
		py = self->wait - self->sy;
	}
	else
	{
		px = self->health - self->sx;
		py = self->delay - self->sy;
	}

	pz = sqrt((px * px) + (py * py));
	if(pz != 0)hyp = speed / pz;
	else hyp = 0;
	self->vx = px * hyp;
	self->vy = py * hyp;
	if(pz < 5)
		if(self->isRight)
			self->isRight = 0;
		else
			self->isRight = 1;

	self->sx += self->vx;
	self->sy += self->vy;

	if(self->vy > 0)
	{
		self->sy -= 2;
		self->bbox.y += 2;
	}
}

Entity *BuildBoundary(int x, int y)
{
	Entity *boundary;
	boundary = NewEntity();
	if(boundary == NULL)return boundary;
	boundary->think = BoundaryThink;
	boundary->isRight = x;
	boundary->delay = y;
	boundary->bbox.x = 0;
	boundary->bbox.y = 0;
	boundary->bbox.w = 64;
	boundary->bbox.h = 720;
	boundary->tang = 1;
	boundary->uTang = 1;
	boundary->lTang = 1;
	boundary->rTang = 1;
	boundary->dTang = 1;
	return boundary;
}

void BoundaryThink(Entity *self)
{
	self->sx = self->isRight + xOffset;
	self->sy = self->delay + yOffset;
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
