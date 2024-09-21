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
#include "structures.h"


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

extern struct collected col;
extern struct sordan_prices sordan_prices;
extern struct wicher wicher;
extern struct rastport_prices rastport_prices;
extern struct stats stats;

extern int spearStatsPerLevel[SPEAR_LEVELS][4];

static tBitMap *s_pTilesEnemies;

UBYTE hudSelectWhat;

char szMsg[50];
UBYTE sordanSelector = CAPACITOR_SELECTION_SORDAN;
UBYTE rastportSelector = LASER_SPEAR_UPGRADE_RASTPORT;

static tFont *s_pFnt;
static tTextBitMap *s_pBmTxt;

static tBitMap *s_pHUD_square;
UBYTE fromMonsterSetSelected;

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

void printOnHUD(char * text, int posX, int posY){
  fontFillTextBitMap(s_pFnt, s_pBmTxt, text);
  fontDrawTextBitMap(s_pMainBuffer->pBack, s_pBmTxt, posX, posY, 4, FONT_COOKIE);
}

void printWhatPlayerHave(){
  blitCopy(s_pHUD_square, 
            20, 50, 
            s_pMainBuffer->pBack, 
            20, 50, 150, 50, MINTERM_COOKIE);
    printOnHUD("You have:", 20, 50);
    sprintf(szMsg, "Energy: %d", col.energy);
    printOnHUD(szMsg, 30, 60);
    sprintf(szMsg, "Coal: %d", col.coal);
    printOnHUD(szMsg, 30, 70);
    sprintf(szMsg, "Capacitors: %d", col.capacitors);
    printOnHUD(szMsg, 30, 80);
    sprintf(szMsg, "Resistors: %d", col.resistors);
    printOnHUD(szMsg, 30, 90);
}

void sordanChangeBuySellSelection(UBYTE selection){
  blitCopy(s_pHUD_square, 30, 120, s_pMainBuffer->pBack, 30, 120, 260, 10, MINTERM_COOKIE);
  blitCopy(s_pHUD_square, 30, 150, s_pMainBuffer->pBack, 30, 150, 260, 10, MINTERM_COOKIE);
  switch (selection){
    case CAPACITOR_SELECTION_SORDAN:
    sprintf(szMsg, "<-  [ 1x Capacitor : %d coals ] ->", sordan_prices.capacitor);
    printOnHUD(szMsg, 30, 120);
    sprintf(szMsg, "<-  [ 1x Capacitor : %d coals ] ->", sordan_prices.scrap);
    printOnHUD(szMsg, 30, 150);
    break;
    case RESISTOR_SELECTION_SORDAN:
    sprintf(szMsg, "<-  [ 1x Resistor : %d coals ]  ->", sordan_prices.resistor);
    printOnHUD(szMsg, 30, 120);
    sprintf(szMsg, "<-  [ 1x Resistor : %d coals ]  ->", sordan_prices.scrap);
    printOnHUD(szMsg, 30, 150);
    break;
  }

}

void rastportChangeUpgradeSelection(UBYTE selection){
  blitCopy(s_pHUD_square, 30, 120, s_pMainBuffer->pBack, 30, 120, 260, 10, MINTERM_COOKIE);
  int lvl;
  switch (selection){
    case LASER_SPEAR_UPGRADE_RASTPORT:
    lvl = stats.spear_level + 1;
    sprintf(szMsg, "<-  [ LASER SPEAR LVL %d ] ->", lvl);
    printOnHUD(szMsg, 30, 120);
    sprintf(szMsg, "%d coals, %d capacitors, %d resistors", spearStatsPerLevel[lvl][1], spearStatsPerLevel[lvl][2], spearStatsPerLevel[lvl][3]);
    printOnHUD(szMsg, 30, 130);

    break;
  }
}



/*
void blitTxtMultiple(int amount){
  if (counter < amount){
    printOnHUD();
    counter++;
  }
}*/

void hudGsCreate(void) {
  systemUse();
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
  s_pTilesEnemies = bitmapCreateFromFile("data/rand_encounter_enemies.bm", 0);
  s_pFnt = fontCreate("data/topaz.fnt");
  s_pBmTxt = fontCreateTextBitMap(320, s_pFnt->uwHeight);

  blitCopy(s_pHUD_square, 
            0, 0, 
            s_pMainBuffer->pBack, 
            0, 0, 320, 256, MINTERM_COOKIE);

//---------------------------------------------------------------- NEW STUFF END

  

  if (hudSelectWhat == HUD_SORDAN){
    printOnHUD("  *** SORDAN ELECTRONICS ***", 40, 20);
    printWhatPlayerHave();

    printOnHUD("(press A) BUY:", 20, 110);
    sordanChangeBuySellSelection(sordanSelector);

    printOnHUD("(press S) SELL:", 20, 140);

  }
  else if (hudSelectWhat == HUD_RASTPORT){
    printOnHUD("      *** RASTPORT ***", 40 ,20);
    printWhatPlayerHave();
    printOnHUD("(press A) UPGRADE:", 20, 110);
    rastportChangeUpgradeSelection(rastportSelector);
  }
  else if (hudSelectWhat == HUD_RANDOM_ENCOUNTER){
    printOnHUD("TEST RANDOM ENCOUNTER", 40, 40); 
    if (fromMonsterSetSelected == 1){
      blitCopy(s_pTilesEnemies, 0, 0, s_pMainBuffer->pBack, 80,80, TS, TS, MINTERM_COOKIE);

    } 
  }
  //keyCreate();

  /*
    this Unuse below causes another systemUse immediately after,
    but without it cannot use keys in hud loop, so let it stay 
    for now and see how to deal with it later
  */
  systemUnuse();

  // Load the view
  viewLoad(s_pViewHUD);
}

void hudGsLoop(void) {
  //blitTxtMultiple(4);
  // This will loop forever until you "pop" or change gamestate
  // or close the game
  if(keyUse(KEY_ESCAPE)){
    statePop(g_pGameStateManager);
    return;
  }

  if (hudSelectWhat == HUD_SORDAN){
    if (keyUse(KEY_LEFT)){
      if (sordanSelector > 1){
        sordanSelector--;
        sordanChangeBuySellSelection(sordanSelector);
      }
    }
    else if (keyUse(KEY_RIGHT)){
      if (sordanSelector < 2){
        sordanSelector++;
        sordanChangeBuySellSelection(sordanSelector);
      }
    }
    else if (keyUse(KEY_A)){
      switch (sordanSelector){
        case CAPACITOR_SELECTION_SORDAN:
          if (col.coal >= sordan_prices.capacitor){
          col.coal -= sordan_prices.capacitor;
          col.capacitors++;
        } 
        break;
        case RESISTOR_SELECTION_SORDAN:
        if (col.coal >= sordan_prices.resistor){
          col.coal -= sordan_prices.resistor;
          col.resistors++;
        }
        break;
      }
      printWhatPlayerHave(); 
    }
    else if(keyUse(KEY_S)){
      switch (sordanSelector){
        case CAPACITOR_SELECTION_SORDAN:
          if (col.capacitors > 0){
            col.capacitors--;
            col.coal++;
          } 
        break;
        case RESISTOR_SELECTION_SORDAN:
        if (col.resistors > 0){
          col.resistors--;
          col.coal++; 
        }
        break;
      }
      printWhatPlayerHave();
    }

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