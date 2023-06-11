#include "game.h"
#include <ace/managers/game.h>
#include <ace/managers/joy.h>
#include <ace/managers/key.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/system.h>
#include <ace/utils/font.h>
#include <ace/utils/palette.h>
//-------------------------------------------------------------- NEW STUFF START
#include <ace/managers/blit.h> // Blitting fns

#include "structures.h"
#include "defines.h"
#include "maps.h"

// Let's make code more readable by giving names to numbers
// It is a good practice to name constant stuff using uppercase
#define BALL_WIDTH 8
#define BALL_COLOR 1
#define PADDLE_WIDTH 8
#define PADDLE_HEIGHT 32
#define PADDLE_LEFT_COLOR 2
#define PADDLE_RIGHT_COLOR 3
#define SCORE_COLOR 1
#define WALL_HEIGHT 1
#define WALL_COLOR 1
//---------------------------------------------------------------- NEW STUFF END

static tView *s_pView; // View containing all the viewports

static tVPort *s_pVpMain; // Viewport for playfield
static tSimpleBufferManager *s_pMainBuffer;

static tBitMap *s_pTileset;
static tFont *s_pFont;
static tTextBitMap *s_pBmText;

struct wicher wicher;
UBYTE direction = DIR_NONE;
int mapCurrent[MAP_HEIGHT][MAP_WIDTH];
UBYTE mapPrep = 0;

void loadMap(int mapSelected[MAP_HEIGHT][MAP_WIDTH])
{ 
	for (UBYTE i = 0; i < MAP_WIDTH; ++i)
	{
		for (UBYTE j = 0; j < MAP_HEIGHT; ++j)
		{
			mapCurrent[j][i] = mapSelected[j][i];
		}
	}
			
}

void drawMap()
{ // todo - pass map name as argument (pointers problems etc)
	for (UBYTE i = 0; i < MAP_WIDTH; ++i)
	{
		for (UBYTE j = 0; j < MAP_HEIGHT; ++j)
		{
			if (mapCurrent[j][i] == EMPTY_TILE)
			{
				blitRect(s_pMainBuffer->pBack, i * TS, j * TS, TS, TS, 0);
			}
			if (mapCurrent[j][i] == METEORITE)
			{
				blitCopy(s_pTileset, 0, 0, s_pMainBuffer->pBack, i * TS, j * TS, TS, TS, MINTERM_COOKIE);
			}
			if (mapCurrent[j][i] == WICHER)
			{
				wicher.mapPosX = i;
				wicher.mapPosY = j;
				wicher.blitPosX = i * TS;
				wicher.blitPosY = j * TS;
				blitCopy(s_pTileset, 0, wicher.face, s_pMainBuffer->pBack, i * TS, j * TS, TS, TS, MINTERM_COOKIE);
			}
		}
	}
}

void mapDrawTwice()
{ // double buffer solution at start of loop
	if (mapPrep < 2)
	{
		drawMap();
		++mapPrep;
	}
}

void gameGsCreate(void) {
  s_pView = viewCreate(0,
    TAG_VIEW_GLOBAL_PALETTE, 1,
    TAG_VIEW_COPLIST_MODE, COPPER_MODE_BLOCK,
  TAG_END);

  // Now let's do the same for main playfield
  s_pVpMain = vPortCreate(0,
    TAG_VPORT_VIEW, s_pView,
    TAG_VPORT_BPP, 5, //4
  TAG_END);
  s_pMainBuffer = simpleBufferCreate(0,
    TAG_SIMPLEBUFFER_VPORT, s_pVpMain,
    TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR | BMF_INTERLEAVED,
										  TAG_SIMPLEBUFFER_IS_DBLBUF, 1,
										  TAG_SIMPLEBUFFER_BOUND_HEIGHT, 512,
										  TAG_SIMPLEBUFFER_BOUND_WIDTH, 1280,
										  TAG_END);

	paletteLoad("data/vk.plt", s_pVpMain->pPalette, 32);
  s_pFont = fontCreate("data/topaz.fnt");
  s_pBmText = fontCreateTextBitMap(300, s_pFont->uwHeight);
	s_pTileset = bitmapCreateFromFile("data/staryWicher.bm", 0);

  wicher.state = STATE_IDLE;
  wicher.face = FACE_RIGHT;
  wicher.animTick = 0;
  wicher.animCount = 0;
  //s_pVpMain->pPalette[2] = 0x0800; // Red - not max, a bit dark
  //blitRect(s_pMainBuffer->pBack, 0, 0, 320, 128, 14);
	//blitRect(s_pMainBuffer->pBack, 0, 128, 320, 128, 8);

  loadMap(map2);
  
  systemUnuse();

  joyOpen();
	keyCreate();
	systemUnuse();
  // Load the view
  viewLoad(s_pView);
}

void gameGsLoop(void) {
  //blitRect(s_pMainBuffer->pBack, 0, 0, 320, 128, 14);
  //drawMap();
  mapDrawTwice();
  // This will loop every frame
  if(keyCheck(KEY_ESCAPE)) {
    gameExit();
  }
  /*else {
//-------------------------------------------------------------- NEW STUFF START
    // Draw first paddle
    blitRect(
      s_pMainBuffer->pBack, 0, 0,
      PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_LEFT_COLOR
    );

//---------------------------------------------------------------- NEW STUFF END
  } */

  //cameraCenterAt(s_pMainBuffer->pCamera, wicher.blitPosX, wicher.blitPosY);
	//viewProcessManagers(s_pView);
	//copProcessBlocks();
	vPortWaitForEnd(s_pVpMain);
}

void gameGsDestroy(void) {
  systemUse();
  joyClose();
	keyDestroy();
  // This will also destroy all associated viewports and viewport managers
  viewDestroy(s_pView);
}