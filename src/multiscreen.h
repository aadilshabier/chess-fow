#ifndef MULTI_SCREEN_H_
#define MULTI_SCREEN_H_

#include "comms.h"
#include "gamestate.h"

void InitMultiScreen(void */*multiplayerModePtr*/);
void UpdateMultiScreen();
void DrawMultiScreen();

extern GameState _MULTISTATEOBJ;
extern Player clientPlayer;

#endif // MULTI_SCREEN_H_
