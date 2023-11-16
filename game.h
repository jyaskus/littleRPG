// Beginning Game Programming, 2nd Edition
// Chapter 8
// Tiled_Sprite program header file


#ifndef GAME_H
#define GAME_H

//windows/directx headers
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#include <dxerr9.h>
#include <dsound.h>
#include <dinput.h>
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <string> 
#include <iostream> 
#include <fstream>
#include <assert.h>
using namespace std;

//framework-specific headers

#include "dxaudio.h"
#include "dxinput.h"
#include "dxgraphics.h"
#include "csprite.h"


/* ------ DEFINE(s) ------ */

//application title
#define APPTITLE "Little RPG"

#define DEBUG_ON false

//screen setup
#define FULLSCREEN 0       //0 = windowed, 1 = fullscreen
#define SCREEN_WIDTH  1024 
#define SCREEN_HEIGHT 768

// tile info
#define TILEWIDTH  64
#define TILEHEIGHT 64      
#define BACK_PER_ROW 4  // number of brackground tiles per row

// map
#define MAPWIDTH   16 // 1024 / 64 = 16 
#define MAPHEIGHT  45 //  768 / 16 = 48
#define GAMEWORLDWIDTH  (TILEWIDTH * MAPWIDTH)
#define GAMEWORLDHEIGHT (TILEHEIGHT * MAPHEIGHT)

// over images in file "bushes.bmp"
#define NUM_OVERLAYS 82
#define MAX_OVERLAYS 50

// respawn delay for monsters
#define DEATH_DELAY 300

// scrolling window size
#define WINDOWWIDTH  (SCREEN_WIDTH / TILEWIDTH)   * TILEWIDTH
#define WINDOWHEIGHT (SCREEN_HEIGHT / TILEHEIGHT) * TILEHEIGHT

// spell casting
#define MANA_FIRE 20
#define MANA_HEAL 10
#define MANA_DELAY 50

typedef enum
{
	SPELL_NONE	=	0,
	SPELL_FIRE	=	1,
	SPELL_HEAL	=	2
}  SPELLS;

/* ------ Structures ------ */

// used for the overlay tiles
struct OverlayType
{
 int x, y; // tile xy
 int tile;
 RECT r1;  // collision rect
};

/* ------ function prototypes ------ */

 int Game_Init(HWND);
void Game_Run(HWND);
void Game_End(HWND);

// dynamic scrolling map support functions
void DrawSprite(LPDIRECT3DTEXTURE9, int,int,int,int, int, int);
void DrawTilesBack();
void DrawSprites();

// test for collisions
bool rectCollision(RECT r1, RECT r2);
bool collisionOverlays(RECT dest_rhombus);
 int getOverlayCollided(RECT dest_rhombus);

// draw numbers to screen
void drawNumbers(int number, int screenX, int screenY);

// MATH
float findDistance(POINT pt1,POINT pt2);

// convert from tile xy to screen xy
POINT tile_to_world(int tilex, int tiley);

// roll dice
int rollDice(int , int);

// draws the game menus
void drawMenu();

// determine which direction to turn to face target
DIRS getDirection(POINT sourcePT, POINT targetPT);

//returns screen coordinates for a given x,y tile location
POINT tile_to_world(int tilex, int tiley);

// returns tileXY where center of rhombus is at (roughly)
POINT sprite_to_tile(RECT rhombus);

#endif
