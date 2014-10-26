
#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "graphics.h"

extern SDL_Surface *screen;
extern SDL_Surface *buffer; /*pointer to the draw buffer*/
//extern SDL_Surface *message;
extern SDL_Rect Camera;

//TTF_Font* font = NULL;
//SDL_Color textColor = {0, 0, 0};


//trying to prevent multiple sprites in one place. What am I even doing?
int spotCheck( int coordCheck, int arrayCheck[], int arraySize, int coord )
{
	int i = 0;
	fprintf(stdout, "spotCheck arraySize %i \n", arraySize);
	while( i < arraySize )
	{
		fprintf(stdout, "i = %i \n", i);
		while( arrayCheck[i] == coordCheck )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
		{
			printf( "!! DUPE !! %i \n", coord );
			i++;
			return 2; //2 means there's a dupe
		}
		while( arrayCheck[i] != coordCheck ){
			printf( "@@ GOOD @@ %i \n ", coord );
			i++;
			return 1; //1 means it's good
		}
		//i++;
		//fprintf( stdout, " %d \n", i );
	}
}

int getImagePathFromFile( char *filepath, char * filename );

void Init_All();

/*this program must be run from the directory directly below images and src, not from within src*/
/*notice the default arguments for main.  SDL expects main to look like that, so don't change it*/
int main( int argc, char *argv[] )
{
	SDL_Surface *temp = NULL;
	SDL_Surface *bg;

	//font = TTF_OpenFont( "fonts/font.ttf", 25);
	//int mess = 100;

	Uint8 *keys;
	char imagepath[512];
	int done;
	int keyn;
	int mx,my;

	Sprite *tile1; //soda
	int spriteLimit1 = 25;		//the amount player can spawn
	int spriteCount1 = 0;		//keeps track of how much has been spawned
	int spriteCount1mx;			//keeps track of the number of clicks for X
	int spriteCount1my;			//keeps track of the number of clicks for y
	int spriteArray1mx[ 25 ];	//keeps track of placed X clicks
	int spriteArray1my[ 25 ];	//keeps track of placed Y clicks
	int mxspot, myspot;

	Sprite *tile2; //cereal
	int spriteLimit2 = 25;
	int spriteArray2[ 25 ];
	int spriteCount2;

	Sprite *tile3; //candy
	int spriteLimit3 = 15;
	int spriteArray3[ 15 ];
	int spriteCount3;

	Sprite *customer;
	int custx = 0;
	int custy = 525;

	Init_All();
	// ------------------------------

	if (getImagePathFromFile(imagepath,"config.ini") == 0)
		temp = IMG_Load(imagepath); /*notice that the path is part of the filename*/
 
	if(temp != NULL) /*ALWAYS check your pointers before you use them*/
		bg = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	if(bg != NULL)
		SDL_BlitSurface(bg,NULL,buffer,NULL);

	tile1 = LoadSprite("images/soda.bmp",32,32); //soda
	tile2 = LoadSprite("images/cereal.bmp",32,32); //cereal
	tile3 = LoadSprite("images/candy.bmp",32,32); //candy
	customer = LoadSprite("images/customer.bmp",32,32); //customer

	//message = TTF_RenderText_Solid( font, "test message", textColor );
	

	done = 0;


	do
	{
		ResetBuffer();
		DrawMouse();
		NextFrame();
		SDL_PumpEvents();
		keys = SDL_GetKeyState(&keyn);

		//soda
		//DrawSprite(Sprite *sprite,SDL_Surface *surface,int sx,int sy, int frame)
		if( (keys[ SDLK_1 ]) && (SDL_GetMouseState( &mx, &my )) && (spriteCount1 < spriteLimit1) )
		{
			printf( " \n ");
			mx = ((mx/32)*32);
			my = ((my/32)*32);

			//check to see if mx, my are already in array
			mxspot = spotCheck( mx, spriteArray1mx, spriteLimit1, mx );
			myspot = spotCheck( my, spriteArray1my, spriteLimit1, my );

			if( mxspot == 1 && myspot == 1 ){ //1 is good, 2 is dupe
				spriteArray1mx[ spriteCount1 ] = mx;
				spriteArray1my[ spriteCount1 ] = my;
				DrawSprite(tile1, buffer, mx, my, 0);
				spriteCount1++;
				fprintf(stdout, "!! mx&my = 1 tile1 remaining: (%i) \n", spriteCount1);
				mxspot = 0;
				myspot = 0;
			}else if( mxspot == 2 && myspot == 2 ){
				printf("!! mx&my = 2 \n ");
				mxspot = 0;
				myspot = 0;
			}else{
				printf("!! nothing. \n");
				mxspot = 0;
				myspot = 0;
			} 
		}
		//cereal
		if( (keys[ SDLK_2 ]) && (SDL_GetMouseState( &mx, &my )) && ( spriteLimit2 > 0 ) )
		{
			DrawSprite(tile2, buffer, (mx/32) * 32, (my/32) * 32, 0);
			spriteLimit2--;
			fprintf(stdout,"tile2 remaining: (%i) \n", spriteLimit2);
		}
		//candy
		if( (keys[ SDLK_3 ]) && (SDL_GetMouseState( &mx, &my )) && ( spriteLimit3 > 0 ) )
		{
			DrawSprite(tile3, buffer, (mx/32) * 32, (my/32) * 32, 0);
			spriteLimit3--;
			fprintf(stdout,"tile3 remaining: (%i) \n", spriteLimit3);
		}
		//customer
		if( (keys[ SDLK_4 ]) )
		{
			DrawSprite(customer, buffer, custx, custy, 0);
			custx = custx + 1;

		}
		if(keys[SDLK_ESCAPE])
		{
			done = 1;
		}
		
		//DrawSprite( NULL, message, 100, 100, 0); 
	}


	while(!done);
	exit(0);	// <-- technically this will end the program, 
	return 0;	// but the compiler likes all functions that can return a 
				// value TO return a value
}

void CleanUpAll()
{
  CloseSprites();
  /*any other cleanup functions can be added here*/ 
}

void Init_All()
{
  Init_Graphics();
  InitMouse();
  atexit(CleanUpAll);
}

int getImagePathFromFile(char *filepath,char * filename)
{
    FILE *fileptr = NULL;
    char buf[255];
    int returnValue = -1;
    if (!filepath){
        fprintf(stdout,"getImagePathFromFile: warning, no output parameter provided\n");
        return -1;
    }
    if (!filename){
        fprintf(stdout,"getImagePathFromFile: warning, no input file path provided\n");
        return -1;
    }
    fileptr = fopen(filename,"r");

    if (!fileptr){
        fprintf(stderr,"unable to open file: %s\n",filename);
        return -1;
    }
    if (fscanf(fileptr,"%s",buf)){
        if (strcmp(buf,"image:")==0){
            fscanf(fileptr,"%s",filepath);
            returnValue = 0;
        }
    }
    fclose(fileptr);
    return returnValue;
}
