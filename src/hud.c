#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/joy.h>
#include <ace/managers/key.h>
#include <ace/managers/system.h>
#include <ace/managers/game.h>
#include <ace/utils/palette.h>
#include <ace/utils/font.h>
#include <stdio.h>
#include <ace/managers/rand.h>
#include <ace/managers/state.h>
#include <ace/utils/custom.h>
#include <ace/managers/ptplayer.h>
#include <ace/utils/file.h>
#include "defines.h"


// All variables outside fns are global - can be accessed in any fn
// Static means here that given var is only for this file, hence 's_' prefix
// You can have many variables with same name in different files and they'll be
// independent as long as they're static
// * means pointer, hence 'p' prefix
static tView *s_pViewHUD; // View containing all the viewports
static tVPort *s_pVpHUD; // Viewport for playfield
static tSimpleBufferManager *s_pMainBuffer;

extern tStateManager *g_pGameStateManager;
extern tState *g_pGameState;
extern tState *g_pHudState;

UBYTE hudSelectWhat;

static tFont *s_pFnt;
static tTextBitMap *s_pBmTxt;

static tBitMap *s_pHUD_square;

UBYTE counter = 0;

void waitFrames(tVPort *pVPort, UBYTE ubHowMany)
{
  for (UBYTE i = 0; i < ubHowMany; ++i)
  {
    viewProcessManagers(pVPort->pView);
    copProcessBlocks();
    vPortWaitForEnd(pVPort);
  }
}

void printOnHUD(char * text){
  fontFillTextBitMap(s_pFnt, s_pBmTxt, text);
  fontDrawTextBitMap(s_pMainBuffer->pBack, s_pBmTxt, 40, 40, 4, FONT_COOKIE);
}

/*
void blitTxtMultiple(int amount){
  if (counter < amount){
    printOnHUD();
    counter++;
  }
}*/

void hudGsCreate(void) {
  s_pViewHUD = viewCreate(0,
    TAG_VIEW_GLOBAL_PALETTE, 1,
  TAG_END);

  // Now let's do the same for main playfield
  s_pVpHUD = vPortCreate(0,
    TAG_VPORT_VIEW, s_pViewHUD,
    TAG_VPORT_BPP, 5,
  TAG_END);
  s_pMainBuffer = simpleBufferCreate(0,
    TAG_SIMPLEBUFFER_VPORT, s_pVpHUD,
    TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR,
  TAG_END);


	paletteLoad("data/vk.plt", s_pVpHUD->pPalette, 32);
  s_pHUD_square = bitmapCreateFromFile("data/hud_square_big.bm", 0);

  s_pFnt = fontCreate("data/topaz.fnt");
  s_pBmTxt = fontCreateTextBitMap(320, s_pFnt->uwHeight);

  blitCopy(s_pHUD_square, 
            0, 0, 
            s_pMainBuffer->pBack, 
            0, 0, 320, 256, MINTERM_COOKIE);

//---------------------------------------------------------------- NEW STUFF END

  

  if (hudSelectWhat == HUD_SORDAN){
    printOnHUD("TEST SORDAN");
  }
  else if (hudSelectWhat == HUD_RASTPORT){
    printOnHUD("TEST RASTPORT");
  }
  else if (hudSelectWhat == HUD_RANDOM_ENCOUNTER){
    printOnHUD("TEST RANDOM ENCOUNTER");  
  }
  //keyCreate();
  //systemUnuse();

  // Load the view
  viewLoad(s_pViewHUD);
}

void hudGsLoop(void) {
  //blitTxtMultiple(4);
  // This will loop forever until you "pop" or change gamestate
  // or close the game
  if(keyUse(KEY_RETURN)) {
    statePop(g_pGameStateManager);
    return;
  }

  waitFrames(s_pVpHUD, 1);
}

void hudGsDestroy(void) {
  // Cleanup when leaving this gamestate
  systemUse();

  // This will also destroy all associated viewports and viewport managers
  viewDestroy(s_pViewHUD);
  systemUnuse();
}