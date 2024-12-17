#ifndef LOADING_SCREEN_H_
#define LOADING_SCREEN_H_

/* #include "comms_client.h" */
#include "gamestate.h"

typedef enum PlayerToggleState {
	PLAYER_TOGGLE_WHITE,
	PLAYER_TOGGLE_RANDOM,
	PLAYER_TOGGLE_BLACK,
} PlayerToggleState;

void InitLoadingScreen();
void UpdateLoadingScreen();
void DrawLoadingScreen();

extern GameState _LOADINGSTATEOBJ;
extern int multiplayerMode;

#endif // LOADING_SCREEN_H_
