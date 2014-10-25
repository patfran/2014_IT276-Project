#include <string.h>
#include <stdlib.h>
#include "graphics.h"

#define MaxSprites    255

struct
{
	Uint32 state;
	Uint32 shown;
	Uint32 frame;
	Uint16  x, y;
}Mouse;

SDL_Surface *screen; /*pointer to the draw buffer*/
SDL_Surface *buffer; /*pointer to the background image buffer*/
SDL_Surface *videobuffer; /*pointer to the actual video surface*/
SDL_Rect Camera; /*x & y are the coordinates for the background map, w and h are of the screen*/
Sprite SpriteList[MaxSprites];
Sprite *Msprite;
int NumSprites;
Uint32 NOW;	/*the current time since program started*/

/*some data on the video settings that can be useful for a lot of functions*/
Uint32 rmask,gmask,bmask,amask;
ScreenData S_Data;


void Init_Graphics()
{
    Uint32 Vflags = /*SDL_FULLSCREEN |*/ SDL_ANYFORMAT;
    Uint32 HWflag = 0;
    SDL_Surface *temp;
    S_Data.xres = 800; //nope
    S_Data.yres = 600;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
    #else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
    #endif
    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_DOUBLEBUF) < 0 )
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
        if(SDL_VideoModeOK(800, 600, 32, /*SDL_FULLSCREEN |*/ SDL_ANYFORMAT | SDL_HWSURFACE))
    {
        S_Data.xres = 800; //<--- this sets the resolution
        S_Data.yres = 600;
        S_Data.depth = 32;
        Vflags = /*SDL_FULLSCREEN |*/ SDL_ANYFORMAT | SDL_HWSURFACE;
        HWflag = SDL_HWSURFACE;
    }
    else if(SDL_VideoModeOK(800, 600, 16, /*SDL_FULLSCREEN |*/ SDL_ANYFORMAT | SDL_HWSURFACE))
    {
        S_Data.xres = 800;
        S_Data.yres = 600;
        S_Data.depth = 16;
        Vflags = /*SDL_FULLSCREEN |*/ SDL_ANYFORMAT | SDL_HWSURFACE;
        HWflag = SDL_HWSURFACE;
    }
    else if(SDL_VideoModeOK(800, 600, 16, /*SDL_FULLSCREEN |*/ SDL_ANYFORMAT))
    {
        S_Data.xres = 800;
        S_Data.yres = 600;
        S_Data.depth = 16;
        Vflags = SDL_FULLSCREEN | SDL_ANYFORMAT;
        HWflag = SDL_SWSURFACE;
    }
    videobuffer = SDL_SetVideoMode(S_Data.xres, S_Data.yres,S_Data.depth, Vflags);
    if ( videobuffer == NULL )
    {
        fprintf(stderr, "Unable to set 800x600 video: %s\n", SDL_GetError());
        exit(1);
    }
    temp = SDL_CreateRGBSurface(SDL_HWSURFACE, S_Data.xres, S_Data.yres, S_Data.depth,rmask, gmask,bmask,amask);
    if(temp == NULL)
	  {
        fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
        exit(1);
	  }
    /* Just to make sure that the surface we create is compatible with the screen*/
    screen = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    temp = SDL_CreateRGBSurface(Vflags, 2048, 768, S_Data.depth,rmask, gmask,bmask,amask);
    if(temp == NULL)
	  {
        fprintf(stderr,"Couldn't initialize Video buffer: %s\n", SDL_GetError());
        exit(1);
	  }
    buffer = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    Camera.x = 0;
    Camera.y = 0;
    Camera.w = screen->w;/*we want to make sure that our camera is the same size of the video screen*/
    Camera.h = screen->h;
    SDL_ShowCursor(SDL_DISABLE);/*don't show the mouse */
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
}

void ResetBuffer()
{
    SDL_BlitSurface(buffer,&Camera,screen,NULL);
}

//set FPS
void NextFrame()
{
  Uint32 Then;
  SDL_BlitSurface(screen,NULL,videobuffer,NULL);/*copy everything we did to the video surface*/
  SDL_Flip(videobuffer);						/*and then update the screen*/
  Then = NOW;									/*these next few lines are used to show how long each frame takes to update.  */
  NOW = SDL_GetTicks();
/*  fprintf(stdout,"Ticks passed this frame: %i\n", NOW - Then);*/
  FrameDelay(33); 
  /*this will make your frame rate about 30 frames per second.  
  If you want 60 fps then set it to about 15 or 16*/
}

/*
 * InitSpriteList is called when the program is first started.
 * It just sets everything to zero and sets all pointers to NULL.
 * It should never be called again.
 */
void InitSpriteList()
{
  int x;
  NumSprites = 0;
  memset(SpriteList,0,sizeof(Sprite) * MaxSprites);
  for(x = 0;x < MaxSprites;x++)SpriteList[x].image = NULL;
}


/*Create a sprite from a file, the most common use for it.*/
Sprite *LoadSprite(char *filename,int sizex, int sizey)
{
	int i;
	SDL_Surface *temp;
	/*first search to see if the requested sprite image is alreday loaded*/
	for(i = 0; i < NumSprites; i++)
	{
		if(strncmp(filename,SpriteList[i].filename,20)==0)
		{
			SpriteList[i].used++;
			return &SpriteList[i];
		}
	}

	/*makesure we have the room for a new sprite*/
	if(NumSprites + 1 >= MaxSprites)
	{
        fprintf(stderr, "Maximum Sprites Reached.\n");
        exit(1);
	}
	/*if its not already in memory, then load it.*/
	NumSprites++;
	for(i = 0;i <= NumSprites;i++)
	{
		if(!SpriteList[i].used)
		break;
	}
	temp = IMG_Load(filename);
	if(temp == NULL)
	{
		fprintf(stderr,"unable to load a vital sprite: %s\n",SDL_GetError());
		exit(0);
	}
	SpriteList[i].image = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	/*sets a transparent color for blitting.*/
	SDL_SetColorKey(SpriteList[i].image, SDL_SRCCOLORKEY , SDL_MapRGB(SpriteList[i].image->format, 255,255,255));
	/*then copy the given information to the sprite*/
	strncpy(SpriteList[i].filename,filename,20);
    /*now sprites don't have to be 16 frames per line, but most will be.*/
	SpriteList[i].framesperline = 16;
	SpriteList[i].w = sizex;
	SpriteList[i].h = sizey;
	SpriteList[i].used++;
	return &SpriteList[i];
}

/*
 * When we are done with a sprite, lets give the resources back to the system...
 * so we can get them again later.
 */
void FreeSprite(Sprite *sprite)
{
	/*first lets check to see if the sprite is still being used.*/
	sprite->used--;
	if(sprite->used == 0)
	{
		strcpy(sprite->filename,"\0");
		/*just to be anal retentive, check to see if the image is already freed*/
		if(sprite->image != NULL)
			SDL_FreeSurface(sprite->image);
		sprite->image = NULL;
	}
 /*and then lets make sure we don't leave any potential seg faults 
  lying around*/
}

void CloseSprites()
{
	int i;
	for(i = 0;i < MaxSprites;i++)
	{
		/*it shouldn't matter if the sprite is already freed, 
		FreeSprite checks for that*/
		FreeSprite(&SpriteList[i]);
	}
}

void DrawSprite(Sprite *sprite,SDL_Surface *surface,int sx,int sy, int frame)
{
    SDL_Rect src,dest;
    src.x = frame%sprite->framesperline * sprite->w;
    src.y = frame/sprite->framesperline * sprite->h;
    src.w = sprite->w;
    src.h = sprite->h;
    dest.x = sx;
    dest.y = sy;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite->image, &src, surface, &dest);
  
}


/*
  makes sure a minimum number of ticks is waited between frames
  this is to ensure that on faster machines the game won't move so fast that
  it will look terrible.
  This is a very handy function in game programming.
*/
void FrameDelay(Uint32 delay)
{
    static Uint32 pass = 100;
    Uint32 dif;
    dif = SDL_GetTicks() - pass;
    if(dif < delay)
		SDL_Delay( delay - dif);
    pass = SDL_GetTicks();
}

/*sets an sdl surface to all color.*/
void BlankScreen(SDL_Surface *buf,Uint32 color)
{
    SDL_LockSurface(buf);
    memset(buf->pixels, (Uint8)color,buf->format->BytesPerPixel * buf->w *buf->h);
    SDL_UnlockSurface(buf);
}



/*mouse handling functions*/
/*this only handles the drawing and animation of.  
Assuming you have a 16 by 16  tiled sprite sheet.  
This will not handle input*/
//------> change mouse sprite <------
void InitMouse()
{
	Msprite = LoadSprite("images/mouse.png",16, 16);
	if(Msprite == NULL)
		fprintf(stdout,"mouse didn't load\n");
	Mouse.state = 0;
	Mouse.shown = 0;
	Mouse.frame = 0;
}


    /*draws to the screen immediately before the blit, after all
     it wouldn't be a very good mouse if it got covered up by the
     game content*/
void DrawMouse()
{
	int mx,my;
	SDL_GetMouseState(&mx,&my);
	if(Msprite != NULL) 
		DrawSprite(Msprite,screen,mx,my,Mouse.frame);
	Mouse.frame = (Mouse.frame + 1)%16;
	Mouse.x = mx;
	Mouse.y = my;
}