#ifndef _ENTITY_
#define _ENTITY_

/*
Functions and stuff for handling entities
*/

#include "graphics.h"

#define MAXENTITIES		255
#define MAXSTATES		20

enum STATES {ST_IDLE, ST_WALK, ST_RUN, ST_SKID, ST_JUMP, ST_FLAP, ST_PUMP, ST_DYING};
enum FORMS {FM_NONE, FM_BALLOON1, FM_BALLOON2};

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
	struct ENTITY_T *below;		/*The entity below this one*/
	struct ENTITY_T *above;		/*The entity above this one*/
	struct ENTITY_T *left;
	struct ENTITY_T *right;
	int isRight;				/*Which way is this facing*/
	int delay;					/*Animation delay*/
	int ct;						/*Counter*/
	int wait;
	int tang;					/*Is this tangible*/
	int uTang, dTang, lTang, rTang;		/*Which directions are tangible*/
	
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
Entity *BuildMovingPlatform(int x, int y, int a, int b);	/*Moving platforms*/
void PlatThink(Entity *self);
Entity *BuildBoundary(int x, int y);
void BoundaryThink(Entity *self);

/*Keyboard Input Stuff*/
void InitKeyboard();
void ClearKeyboard();
void UpdateKeyboard();
int isKeyPressed(int key);
int isKeyReleased(int key);
int isKeyHeld(int key);

#endif