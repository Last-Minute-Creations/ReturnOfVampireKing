#include "hud.h"
#include <ace/managers/key.h> // Keyboard processing
#include <ace/managers/game.h> // For using gameExit
#include <ace/utils/palette.h>
#include <ace/managers/state.h>
#include <ace/managers/system.h> // For systemUnuse and systemUse
#include <ace/managers/viewport/simplebuffer.h> // Simple buffer
#include <ace/managers/blit.h>

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

void hudGsCreate(void) {
  s_pViewHUD = viewCreate(0,
    TAG_VIEW_GLOBAL_PALETTE, 1,
  TAG_END);

  // Now let's do the same for main playfield
  s_pVpHUD = vPortCreate(0,
    TAG_VPORT_VIEW, s_pViewHUD,
    TAG_VPORT_BPP, 4,
  TAG_END);
  s_pMainBuffer = simpleBufferCreate(0,
    TAG_SIMPLEBUFFER_VPORT, s_pVpHUD,
    TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR,
  TAG_END);


	paletteLoad("data/vk.plt", s_pVpHUD->pPalette, 32);


  blitRect(
    s_pMainBuffer->pBack,
    0, 0,
    64, 64, 3
  );
//---------------------------------------------------------------- NEW STUFF END

  systemUnuse();

  // Load the view
  viewLoad(s_pViewHUD);
}

void hudGsLoop(void) {
  // This will loop forever until you "pop" or change gamestate
  // or close the game
  if(keyUse(KEY_RETURN)) {
    statePop(g_pGameStateManager);
    return;
  }
  else {
    // Process loop normally
    // We'll come back here later
  }
}

void hudGsDestroy(void) {
  // Cleanup when leaving this gamestate
  systemUse();

  // This will also destroy all associated viewports and viewport managers
  viewDestroy(s_pViewHUD);
}