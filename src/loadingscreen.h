#ifndef LOADING_SCREEN_H_
#define LOADING_SCREEN_H_

#include "raylib.h"

#include "gamestate.h"

void InitLoadingScreen();
void UpdateLoadingScreen();
void DrawLoadingScreen();

extern GameState _LOADINGSTATEOBJ;

#endif // LOADING_SCREEN_H_
