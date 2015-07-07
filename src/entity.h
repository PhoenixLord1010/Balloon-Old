#ifndef _ENTITY_
#define _ENTITY_

/*
Functions and stuff for handling entities
*/

#include "graphics.h"

#define MAXENTITIES		255
#define MAXSTATES		20

enum STATES {ST_IDLE, ST_WALK, ST_RUN, ST_SKID, ST_JUMP, ST_FLAP, ST_DYING};
enum FORMS {FM_NONE, FM_BALLOON};

typedef struct ENTITY_T
{
	Sprite *sprite;				/*The sprite to be drawn*/
	struct ENTITY_T *owner;		/*For stuff spawned by entities*/
	void (*think) (struct ENTITY_T *self);	/*Called by engine to handle inputs*/
	int used;					/*Is this entity free*/
	int shown;					/*Should this be rendered*/
	int layer;					/*What layer is this on*/
	int frame;					/*Current frame to render*/
	int state;					/*What state is this in*/
	int form;					/*What form is the player in*/
	float sx,sy;				/*Position*/
	float vx,vy;				/*Velocity*/
	SDL_Rect bbox;				/*Bounding box for collisions*/
	
	int uCheck, dCheck, lCheck, rCheck;		/*Collision checks*/
	int isRight;				/*Which way is this facing*/
	int delay;					/*Animation delay*/
	int tang;					/*Is this tangible*/
	
	int health;					/*Current health*/
}Entity;

/*General Entity Stuff*/
void InitEntityList();
void ClearEntities();
void ClearEntitiesExcept(Entity *skip);
Entity *NewEntity();
void FreeEntity(Entity *ent);
void DrawEntity(Entity *ent);
void DrawEntities();
void UpdateEntities();

/*Specific Entity Stuff*/
Entity *MakePlayer(int x, int y);
void PlayerThink(Entity *self);
Entity *MakeBalloon();
void BalloonThink(Entity *self);
Entity *BuildBrick(int x, int y);
Entity *BuildColumn(int x, int y);
void ObjectThink(Entity *self);
void BuildRoad(int x, int y, int i);

/*Keyboard Input Stuff*/
void InitKeyboard();
void ClearKeyboard();
void UpdateKeyboard();
int isKeyPressed(int key);
int isKeyReleased(int key);
int isKeyHeld(int key);

#endif