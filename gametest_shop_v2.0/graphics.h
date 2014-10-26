#ifndef _GRAPHICS_
#define _GRAPHICS_
/*
 *			graphics.h
 *			Originaly written by Donald Kehoe for the NJIT Introduction to game development class
 *			This file contains data structures, constants and function prototypes needed to initialize SDL and
 *			to handle sprites and graphics.
 */


#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"


typedef struct Sprite_T
{
	SDL_Surface *image;		/*pointer to the actual image in memory*/
	char filename[20];			/*the name of the file, keep them short*/
	int w, h;					/*the width and height of the frames of the sprites, not the file itself*/
	int framesperline;			/*default is 16*/
	int used;					/*used by the maintanenc functions to keep track of how many times a single sprite is being used*/
}Sprite;

typedef struct	/*this didn't need to be its own type, its only used once*/
{
    int xres,yres,depth;
}ScreenData;

void Init_Graphics();
void BlankScreen(SDL_Surface *buf,Uint32 color);
void InitSpriteList();
void FreeSprite(Sprite *img);
Sprite *LoadSprite(char *filename,int sizex, int sizey);		/*simplest file to load*/

/*drawsprite and drawspritepart work the same except that Drawsprite 
  will render an entire sprite on the desired srface
  and drawspritepart can have the size of the sprite specified.  
  Know your sprite when using DSP since you can in theory
  read too far, not that SDL will allow that.*/
void DrawSprite(Sprite *sprite,SDL_Surface *surface,int sx,int sy, int frame);
void CloseSprites(); 
	/*call this before you exit to make sure all the 
	memory that your program used is given back to your OS*/

void FrameDelay(Uint32 delay);
void ResetBuffer();
void NextFrame();
void InitMouse();
void DrawMouse();

#endif
