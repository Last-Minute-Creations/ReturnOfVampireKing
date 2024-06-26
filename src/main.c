#include <ace/generic/main.h>
#include <ace/managers/key.h>
#include <ace/managers/state.h>
// Without it compiler will yell about undeclared gameGsCreate etc
#include "game.h"
#include "hud.h"

tStateManager *g_pGameStateManager = 0;
tState *g_pGameState = 0;
tState *g_pHudState = 0;

void genericCreate(void) {
  // Here goes your startup code
  keyCreate(); // We'll use keyboard
  // Initialize gamestate
  g_pGameStateManager = stateManagerCreate();
  g_pGameState = stateCreate(gameGsCreate, gameGsLoop, gameGsDestroy, 0, gameOnResume, 0);
  g_pHudState = stateCreate(hudGsCreate, hudGsLoop, hudGsDestroy, 0, 0, 0);
  statePush(g_pGameStateManager, g_pGameState);
}

void genericProcess(void) {
  // Here goes code done each game frame
  keyProcess();
  stateProcess(g_pGameStateManager); // Process current gamestate's loop
}

void genericDestroy(void) {
  // Here goes your cleanup code
  stateManagerDestroy(g_pGameStateManager);
  stateDestroy(g_pGameState);
  keyDestroy(); // We don't need it anymore
}