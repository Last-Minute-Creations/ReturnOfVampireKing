#include <ace/managers/state.h>
#include <ace/managers/viewport/simplebuffer.h>
#include "hud.h"

extern tStateManager *g_pStateMachineGame;

static tView *s_pHUDView;





tState g_sStateGame = {
    .cbCreate = stateHUDCreate,
    .cbLoop = stateHUDLoop,
    .cbDestroy = stateHUDDestroy,
    .cbSuspend = 0,
    .cbResume = 0,
    .pPrev = 0};