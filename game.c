#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/joy.h>
#include <ace/managers/key.h>
#include <ace/managers/system.h>
#include <ace/managers/game.h>
#include <ace/utils/palette.h>
#include <ace/utils/font.h>
#include <stdio.h>
#include <ace/managers/state.h>
#include <ace/managers/rand.h>
#include "defines.h"
#include "structures.h"
#include "maps.h"
#include "anim.h"

static tView *s_pView;
static tVPort *s_pVp;
static tSimpleBufferManager *s_pVpManagerGame;
static tBitMap *s_pTileset;
static tFont *s_pFont;
static tTextBitMap *s_pBmText;

extern tState g_sStateGame;
extern tState g_sStateCombat;
extern tStateManager *g_pStateMachineGame;

struct wicher wicher;

UBYTE direction = DIR_NONE;
UBYTE interactionType = 0;
BOOL executeInteractionCheck = FALSE;

int mapCurrent[MAP_HEIGHT][MAP_WIDTH];
UBYTE mapPrep = 0;

void gameOnResume(void)
{
  viewLoad(s_pView);
}

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

void updateWicherPositionOnMapAfterAnim(UBYTE dir)
{
	switch (dir)
	{
	case RIGHT:
		wicher.mapPosX++;
		break;
	case LEFT:
		wicher.mapPosX--;
		break;
	case UP:
		wicher.mapPosY--;
		break;
	case DOWN:
		wicher.mapPosY++;
		break;

	default:
		break;
	}
}

void isTileWalkableCheckAndPass(UBYTE dir)
{
	if (dir != DIR_NONE)
	{
		switch (dir)
		{
		case RIGHT:
			if (mapCurrent[wicher.mapPosY][wicher.mapPosX + 1] == 0)
			{
				wicher.state = STATE_ANIM;
			}
			break;
		case LEFT:
			if (mapCurrent[wicher.mapPosY][wicher.mapPosX - 1] == 0)
			{
				wicher.state = STATE_ANIM;
			}
			break;
		case UP:
			if (mapCurrent[wicher.mapPosY - 1][wicher.mapPosX] == 0)
			{
				wicher.state = STATE_ANIM;
			}
			break;
		case DOWN:
			if (mapCurrent[wicher.mapPosY + 1][wicher.mapPosX] == 0)
			{
				wicher.state = STATE_ANIM;
			}
			break;

		default:
			break;
		}

		blitRect(s_pVpManagerGame->pBack, wicher.blitPosX, wicher.blitPosY, TS, TS, 0);
		blitCopy(s_pTileset, 0, wicher.face, s_pVpManagerGame->pBack, wicher.blitPosX, wicher.blitPosY, TS, TS, MINTERM_COOKIE);

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
				blitRect(s_pVpManagerGame->pBack, i * TS, j * TS, TS, TS, 0);
			}
			if (mapCurrent[j][i] == METEORITE)
			{
				blitCopy(s_pTileset, 0, 0, s_pVpManagerGame->pBack, i * TS, j * TS, TS, TS, MINTERM_COOKIE);
			}
			if (mapCurrent[j][i] == WICHER)
			{
				wicher.mapPosX = i;
				wicher.mapPosY = j;
				wicher.blitPosX = i * TS;
				wicher.blitPosY = j * TS;
				blitCopy(s_pTileset, 0, wicher.face, s_pVpManagerGame->pBack, i * TS, j * TS, TS, TS, MINTERM_COOKIE);
			}
			if (mapCurrent[j][i] == FALKON)
			{
				blitCopy(s_pTileset, 160, 32, s_pVpManagerGame->pBack, i * TS, j * TS, TS, TS, MINTERM_COOKIE);	
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
		// after drawing map twice and have the position of Wicher set
		// clear the starting position on map to 0 so it becomes walkable 
		if (mapPrep == 2){
		mapCurrent[wicher.mapPosY][wicher.mapPosX] = 0;
		}
	}
}

void checkInteraction(UBYTE dir){
switch (dir)
	{
		case RIGHT:
			if (mapCurrent[wicher.mapPosY][wicher.mapPosX + 1] != 0)
			{
				interactionType = mapCurrent[wicher.mapPosY][wicher.mapPosX + 1];
			}
			break;
		case LEFT:
			if (mapCurrent[wicher.mapPosY][wicher.mapPosX - 1] != 0)
			{
				interactionType = mapCurrent[wicher.mapPosY][wicher.mapPosX - 1];
			}
			break;
		case UP:
			if (mapCurrent[wicher.mapPosY - 1][wicher.mapPosX] != 0)
			{
				interactionType = mapCurrent[wicher.mapPosY - 1][wicher.mapPosX];
			}
			break;
		case DOWN:
			if (mapCurrent[wicher.mapPosY + 1][wicher.mapPosX] != 0)
			{
				interactionType = mapCurrent[wicher.mapPosY + 1][wicher.mapPosX];
			}
			break;

		default:
			break;	
	}
	if (interactionType != 0){
		executeInteractionCheck = TRUE;
	}
}

void executeInteraction(UBYTE type){
	if (type == FALKON){
		// test interaction squae  WORKS
		//blitRect(s_pVpManagerGame->pBack, 32, 32, 96, 96, randUwMinMax(0, 0, 32));
		// test state change
		statePush(g_pStateMachineGame, &g_sStateCombat);
	}
	
}
////////////////////////////////////////////////////
void stateGameCreate(void)
{
	s_pView = viewCreate(0,
						 TAG_VIEW_COPLIST_MODE, COPPER_MODE_BLOCK,
						 TAG_VIEW_GLOBAL_CLUT, 1,
						 TAG_END);
	s_pVp = vPortCreate(0,
						TAG_VPORT_VIEW, s_pView,
						TAG_VPORT_BPP, 5,
						TAG_END);

	s_pVpManagerGame = simpleBufferCreate(0,
										  TAG_SIMPLEBUFFER_VPORT, s_pVp,
										  TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR | BMF_INTERLEAVED,
										  TAG_SIMPLEBUFFER_IS_DBLBUF, 1,
										  TAG_SIMPLEBUFFER_BOUND_HEIGHT, 512,
										  TAG_SIMPLEBUFFER_BOUND_WIDTH, 1280,
										  TAG_END);

	paletteLoad("data/falkon.plt", s_pVp->pPalette, 32);
	// messages will be added later:
	s_pFont = fontCreate("data/topaz.fnt");
	s_pBmText = fontCreateTextBitMap(320, s_pFont->uwHeight);
	s_pTileset = bitmapCreateFromFile("data/staryWicher.bm", 0);

	//blitRect(s_pVpManagerGame->pBack, 0, 0, 320, 128, 0);
	//blitRect(s_pVpManagerGame->pBack, 0, 128, 320, 128, 0);

	//////////////// Setup

	wicher.state = STATE_IDLE;
    wicher.face = FACE_RIGHT;
    wicher.animTick = 0;
    wicher.animCount = 0; 

	loadMap(map1);
	
	randInit(0, 69, 1337);

	joyOpen();
	keyCreate();
	systemUnuse();
	viewLoad(s_pView);
}
///////////////////////////////////////////////////////
void stateGameLoop(void)
{
	mapDrawTwice();
	isTileWalkableCheckAndPass(direction);

	if (executeInteractionCheck == TRUE){
		executeInteraction(interactionType);
		executeInteractionCheck = FALSE;
		interactionType = 0;
	}

	if (wicher.state == STATE_ANIM)
	{
		blitWicherAnim(direction, s_pVpManagerGame->pBack, s_pTileset);
		++wicher.animTick;
		if (wicher.animTick == 4)
		{
			++wicher.animCount;
			wicher.animTick = 0;
		}
		if (wicher.animCount > 8)
		{
			wicher.state = STATE_IDLE;
			updateWicherPositionOnMapAfterAnim(direction);
			direction = DIR_NONE;
			wicher.animCount = 0;
		}
	}

	if (wicher.state == STATE_IDLE)
	{

		joyProcess();
		keyProcess();

		if (keyUse(KEY_ESCAPE)){
			gameExit();
			return;
		}

		else if (keyCheck(KEY_RIGHT))
		{
			direction = RIGHT;
			wicher.face = FACE_RIGHT;
		}
		else if (keyCheck(KEY_LEFT))
		{
			direction = LEFT;
			wicher.face = FACE_LEFT;
		}
		else if (keyCheck(KEY_UP))
		{
			direction = UP;
			wicher.face = FACE_UP;
		}
		else if (keyCheck(KEY_DOWN))
		{
			direction = DOWN;
			wicher.face = FACE_DOWN;
		}
		else if (keyCheck(KEY_RETURN)){
			checkInteraction(direction);
		}
	}

	cameraCenterAt(s_pVpManagerGame->pCamera, wicher.blitPosX, wicher.blitPosY);
	viewProcessManagers(s_pView);
	copProcessBlocks();
	vPortWaitForEnd(s_pVp);
}

void stateGameDestroy(void)
{

	systemUse();
	viewDestroy(s_pView);
	joyClose();
	keyDestroy();
}

tState g_sStateGame = {
	.cbCreate = stateGameCreate,
	.cbLoop = stateGameLoop,
	.cbDestroy = stateGameDestroy,
	.cbSuspend = 0,
	.cbResume = gameOnResume,
	.pPrev = 0};
