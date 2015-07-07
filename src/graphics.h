#ifndef _GRAPHICS_
#define _GRAPHICS_

/*
Functions for handling graphical stuff, like sprites and fonts
*/

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "GL\glew.h"

/*Color Constants*/
enum colors {Red = 1,Green = 2,Blue = 3,Yellow = 4,Orange = 5,Violet = 6,Brown = 7,Grey = 8,DarkRed = 9,DarkGreen = 10,
               DarkBlue = 11,DarkYellow = 12,DarkOrange = 13,DarkViolet = 14,DarkBrown = 15,DarkGrey = 16,LightRed =17,
               LightGreen = 18,LightBlue = 19,LightYellow = 20,LightOrange = 21,LightViolet = 22,LightBrown = 23,LightGrey = 24,
               Black = 25,White = 26,Tan = 27,Gold = 28,Silver = 29,YellowGreen = 30,Cyan = 31,Magenta = 32};

enum FONTS {F_Small, F_Medium, F_Large};

typedef struct COORD_T
{
  float x,y,z;
}Coord;

typedef struct Sprite_T
{
	char filename[80];
	SDL_Surface *surface;
	int color1, color2, color3;
	int w, h;				/*width and height of individual frames*/
	int framesperline;			/*default is 16*/
	int used;				/*how many times a single sprite's been loaded*/
	int loaded;				/*is a sprite being used at all*/
	int numframes;
}Sprite;

typedef struct
{
	int xres, yres, depth;
}ScreenData;

/*Basics*/
void Init_Graphics(int windowed);
void DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, int x, int y);
void DrawSquareLine(SDL_Surface *screen,Uint32 color,float sx,float sy,float gx,float gy);
void ShowImage(SDL_Surface *image, SDL_Surface *screen, float x, float y);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void DrawFilledRect(float sx, float sy, int sw, int sh, Uint32 Color, SDL_Surface *surface);
void DrawRect(float sx, float sy, int sw, int sh, Uint32 Color, SDL_Surface *surface);

/*Sprite Functions*/
void InitSpriteList();
void FreeSprite(Sprite *img);
Sprite *LoadSprite(char *filename, int sizex, int sizey);
void DrawSprite(Sprite *sprite, SDL_Surface *surface, float sx, float sy, int frame);
void DrawSpritePixel(Sprite *sprite, SDL_Surface *surface, float sx, float sy, int frame);
void CloseSprites();

/*Palette Swapping Functions*/
Sprite *LoadSwappedSprite(char *filename,int sizex, int sizey, int c1, int c2, int c3);
Uint32 SetColor(Uint32 color, int newcolor1,int newcolor2, int newcolor3);
void SwapSprite(SDL_Surface *sprite,int color1,int color2,int color3);
Uint32 IndexColor(int color);

/*Frame Handling Functions*/
void BlankScreen(SDL_Surface *buf, Uint32 color);
void FrameDelay(Uint32 delay);
void ResetBuffer();
void NextFrame();

/*Font Stuff*/
void LoadFonts();
void LoadFont(char filename[40],int ptsize,int type);
void DrawText(char *text,SDL_Surface *surface,int sx,int sy,Uint32 color,int size);
void DrawTextCentered(char *text,SDL_Surface *surface,int sx,int sy,Uint32 color,int size);
void DrawTextBlock(char *thetext,SDL_Surface *surface,int sx, int sy,Uint32 color,int size,int width);
void DrawMessages();
void InitMessages();
void NewMessage(char *text,Uint32 color);


#endif