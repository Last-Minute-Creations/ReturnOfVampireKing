#include "game.h"
#include <ace/managers/game.h>
#include <ace/managers/joy.h>
#include <ace/managers/key.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/system.h>
#include <ace/managers/state.h>
#include <ace/utils/font.h>
#include <ace/utils/palette.h>
#include <ace/managers/blit.h> // Blitting fns

#include "structures.h"
#include "defines.h"
#include "maps.h"


//extern tStateManager *g_pStateMachineGame;
extern tStateManager *g_pGameStateManager;
extern tState *g_pHudState;
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

extern UBYTE hudSelectWhat;

int blitSquareHUDtwice = 2;

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
void blitWicherAnim(UBYTE dir)
{
	switch (dir) // setting up 'pixel position' for blitting fly animation
	{				   // based on position on level map array, every frame
	case RIGHT:
		wicher.blitPosX = (wicher.mapPosX * TS) + (wicher.animCount* WICHER_ANIM_SPEED);
		break;
	case LEFT:
		wicher.blitPosX = (wicher.mapPosX * TS) - (wicher.animCount * WICHER_ANIM_SPEED);
		break;
	case UP:
		wicher.blitPosY = (wicher.mapPosY * TS) - (wicher.animCount * WICHER_ANIM_SPEED);
		break;
	case DOWN:
		wicher.blitPosY = (wicher.mapPosY * TS) + (wicher.animCount* WICHER_ANIM_SPEED);
		break;
	default:
		break;
	}

	if (wicher.animTick == 0 || wicher.animTick == 1)
	{
		if (wicher.animCount == 0)
		{	// this might be unnecessary
			// blitRect(s_pVpManagerMenu->pBack,blitPosX,blitPosY,TS,TS,0);
			// blitCopy(s_pTileset,0,wicherFace,s_pVpManagerMenu->pBack,blitPosX,blitPosY,TS,TS,MINTERM_COOKIE);
		}
		else
		{
			switch (dir) // this is for proper background bliting each frame
			{				   // blits bg on the oposite side than direction
			case RIGHT:
				blitRect(s_pMainBuffer->pBack, wicher.blitPosX - WICHER_ANIM_SPEED, wicher.blitPosY, TS, TS, 0);
				break;
			case LEFT:
				blitRect(s_pMainBuffer->pBack, wicher.blitPosX + WICHER_ANIM_SPEED, wicher.blitPosY, TS, TS, 0);
				break;
			case UP:
				blitRect(s_pMainBuffer->pBack, wicher.blitPosX, wicher.blitPosY + WICHER_ANIM_SPEED, TS, TS, 0);
				break;
			case DOWN:
				blitRect(s_pMainBuffer->pBack, wicher.blitPosX, wicher.blitPosY - WICHER_ANIM_SPEED, TS, TS, 0);
				break;

			default:
				break;
			}
			blitCopy(s_pTileset, 0, wicher.face, s_pMainBuffer->pBack, wicher.blitPosX, wicher.blitPosY, TS, TS, MINTERM_COOKIE);
		}
	}
}


void isInteractionOnAdjacentTile(){
	switch(mapCurrent[wicher.mapPosY][wicher.mapPosX]){
		case 's':
		hudSelectWhat = HUD_SORDAN;
		statePush(g_pGameStateManager, g_pHudState);
		break;
		case 'r':
		hudSelectWhat = HUD_RASTPORT;
		statePush(g_pGameStateManager, g_pHudState);
		break;
	}
}

BOOL walkableTiles(int checkX, int checkY){
	switch (mapCurrent[checkY][checkX]){
		case 0:
		case s:
		case r:
		return TRUE;
	}
	return FALSE;
}

void isTileWalkableCheckAndPass(UBYTE dir)
{
	if (dir != DIR_NONE)
	{
		switch (dir)
		{
		case RIGHT:
			if (walkableTiles(wicher.mapPosX + 1, wicher.mapPosY))
			//if (mapCurrent[wicher.mapPosY][wicher.mapPosX + 1] == 0 || mapCurrent[wicher.mapPosY][wicher.mapPosX + 1] == s) // TODO BETTER
			{
				wicher.state = STATE_ANIM;
			}
			break;
		case LEFT:
			if (walkableTiles(wicher.mapPosX - 1, wicher.mapPosY))
			//if (mapCurrent[wicher.mapPosY][wicher.mapPosX - 1] == 0 || mapCurrent[wicher.mapPosY][wicher.mapPosX - 1] == s) // TODO BETTER)
			{
				wicher.state = STATE_ANIM;
			}
			break;
		case UP:
			if (walkableTiles(wicher.mapPosX, wicher.mapPosY - 1))
			//if (mapCurrent[wicher.mapPosY - 1][wicher.mapPosX] == 0  || mapCurrent[wicher.mapPosY -1][wicher.mapPosX] == s) // TODO BETTER)
			{
				wicher.state = STATE_ANIM;
			}
			break;
		case DOWN:
			if (walkableTiles(wicher.mapPosX, wicher.mapPosY + 1))
			//if (mapCurrent[wicher.mapPosY + 1][wicher.mapPosX] == 0  || mapCurrent[wicher.mapPosY + 1][wicher.mapPosX] == s) // TODO BETTER)
			{
				wicher.state = STATE_ANIM;
			}
			break;

		default:
			break;
		}
	}
}

void drawMap()
{ // todo - pass map name as argument (pointers problems etc)
	for (UBYTE i = 0; i < MAP_WIDTH; ++i)
	{
		for (UBYTE j = 0; j < MAP_HEIGHT; ++j)
		{
			if (mapCurrent[j][i] == EMPTY_TILE || mapCurrent[j][i] == s || mapCurrent[j][i] == r) // TODO BETTER
			{
				blitRect(s_pMainBuffer->pBack, i * TS, j * TS, TS, TS, 0);
			}
			if (mapCurrent[j][i] == S){ // SORDAN TILE
				blitCopy(s_pTileset, 160, 32, s_pMainBuffer->pBack, i * TS, j * TS, TS, TS, MINTERM_COOKIE);
			}
			if (mapCurrent[j][i] == R){ // RASTPORT TILE
				blitCopy(s_pTileset, 192, 32, s_pMainBuffer->pBack, i * TS, j * TS, TS, TS, MINTERM_COOKIE);
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
        if (mapPrep == 1){
          mapCurrent[j][i] = EMPTY_TILE;
        } 
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

  //loadMap(map2);
  loadMap(map_commodorlen);
 
  
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
  
  isTileWalkableCheckAndPass(direction);

  if (wicher.state == STATE_ANIM)
	{
		blitWicherAnim(direction);
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

		else if (keyUse(KEY_RETURN)){
			isInteractionOnAdjacentTile();
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
		}
		else if (keyCheck(KEY_DOWN))
		{
			direction = DOWN;
		}
	}

	if (wicher.state == STATE_HUD){
		if (keyUse(KEY_RETURN)){
			wicher.state = STATE_IDLE;
		}
	}

  cameraCenterAt(s_pMainBuffer->pCamera, wicher.blitPosX, wicher.blitPosY);
	viewProcessManagers(s_pView);
	copProcessBlocks();
	vPortWaitForEnd(s_pVpMain);
}

void gameGsDestroy(void) {
  systemUse();
  joyClose();
	keyDestroy();
  // This will also destroy all associated viewports and viewport managers
  viewDestroy(s_pView);
}
/*
tState g_sStateGame = {
    .cbCreate = stateGameCreate,
    .cbLoop = stateGameLoop,
    .cbDestroy = stateGameDestroy,
    .cbSuspend = 0,
    .cbResume = gameOnResume,
    .pPrev = 0};
	*/