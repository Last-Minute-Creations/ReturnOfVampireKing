#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/joy.h>
#include <ace/managers/key.h>
#include <ace/managers/system.h>
#include <ace/managers/game.h>
#include <ace/utils/palette.h>
#include <stdio.h>
#include <ace/managers/rand.h>
#include <ace/managers/state.h>
#include <ace/managers/blit.h>
#include <ace/utils/font.h>
#include <ace/managers/ptplayer.h>
#include "structures.h"

#define TEXT_BITMAP_WIDTH 320
#define PLAYER_STATS_BLIT_X 10
#define OPP_STATS_BLIT_X 140

static tView *s_pView;
static tVPort *s_pVp;
static tSimpleBufferManager *s_pVpManager;

extern tState g_sStateGame;
extern tStateManager *g_pStateMachineGame;

static tFont *s_pFont;
static tTextBitMap *s_pBmText;

extern struct wicher wicher;
extern struct stats opponent;
extern struct stats player;
char msg[50];

void waitFrames(tVPort *pVPort, UBYTE ubHowMany)
{
  for (UBYTE i = 0; i < ubHowMany; ++i)
  {
    viewProcessManagers(pVPort->pView);
    copProcessBlocks();
    vPortWaitForEnd(pVPort);
  }
}

void showStats(int posX, struct stats *stats){
    blitRect(s_pVpManager->pBack, posX, 30, 110, 50, 22);
    sprintf(msg, "Power     : %d", stats->power);
    fontFillTextBitMap(s_pFont, s_pBmText, msg);
    fontDrawTextBitMap(s_pVpManager->pBack, s_pBmText, posX, 30, 23, FONT_COOKIE);
    sprintf(msg, "Endurance  : %d", stats->endurance);
    fontFillTextBitMap(s_pFont, s_pBmText, msg);
    fontDrawTextBitMap(s_pVpManager->pBack, s_pBmText, posX, 40, 23, FONT_COOKIE);
    sprintf(msg, "Energy    : %d", stats->energy);
    fontFillTextBitMap(s_pFont, s_pBmText, msg);
    fontDrawTextBitMap(s_pVpManager->pBack, s_pBmText, posX, 50, 23, FONT_COOKIE);
}

void attack(){
  UBYTE attackRoll = randUlMinMax(0,1,6);
  UBYTE defenceRoll = randUlMinMax(0,1,6);
  if (player.power + attackRoll > opponent.power + defenceRoll){
    opponent.endurance -= (player.power + attackRoll) - (opponent.power + defenceRoll);
  }
  showStats(OPP_STATS_BLIT_X, &opponent);
}

void stateCombatCreate(void)
{
  systemUse();

  s_pView = viewCreate(0,
                       TAG_VIEW_COPLIST_MODE, COPPER_MODE_BLOCK,
                       TAG_VIEW_GLOBAL_CLUT, 1,
                       TAG_END);

  s_pVp = vPortCreate(0,
                      TAG_VPORT_VIEW, s_pView,
                      TAG_VPORT_BPP, 5,
                      TAG_END);
  // Paleta z falkona
  paletteLoad("data/falkon.plt", s_pVp->pPalette, 32);

  s_pVpManager = simpleBufferCreate(0,
                                    TAG_SIMPLEBUFFER_VPORT, s_pVp,
                                    TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR | BMF_INTERLEAVED,
                                    TAG_SIMPLEBUFFER_IS_DBLBUF, 0,
                                    TAG_END);

  s_pFont = fontCreate("data/topaz.fnt");
  s_pBmText = fontCreateTextBitMap(TEXT_BITMAP_WIDTH, s_pFont->uwHeight);

  systemUnuse();

    // test screen rect like in falcon
  blitRect(s_pVpManager->pBack, 0, 0, 320, 128, 21);
  blitRect(s_pVpManager->pBack, 0, 128, 320, 128, 21);

    fontFillTextBitMap(s_pFont, s_pBmText, "KOMBAT HERE !");
    fontDrawTextBitMap(s_pVpManager->pBack, s_pBmText, 10, 10, 23, FONT_COOKIE);

    showStats(PLAYER_STATS_BLIT_X, &player);
    showStats(OPP_STATS_BLIT_X, &opponent);

    viewLoad(s_pView);
}

void stateCombatLoop(void)
{
  joyProcess();
  keyProcess();

  if (joyUse(JOY1_FIRE) || keyUse(KEY_RETURN))
  {
    statePop(g_pStateMachineGame);
    return;
  }
  if (keyUse(KEY_A)){
    attack();
  }

  viewProcessManagers(s_pView);
  copProcessBlocks();
  waitFrames(s_pVp, 1);
}

void stateCombatDestroy(void)
{
  systemUse();
  viewDestroy(s_pView);
  systemUnuse();
}

tState g_sStateCombat = {
    .cbCreate = stateCombatCreate,
    .cbLoop = stateCombatLoop,
    .cbDestroy = stateCombatDestroy,
    .cbSuspend = 0,
    .cbResume = 0,
    .pPrev = 0};
