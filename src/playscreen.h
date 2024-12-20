#ifndef PLAY_SCREEN_H_
#define PLAY_SCREEN_H_

#include "raylib.h"

#include "board.h"
#include "gamestate.h"

extern Texture2D spriteTexture;
extern const int cellSize;
extern Board board;

extern GameState _PLAYSTATEOBJ;

void InitPlayScreen(void *playerPtr);
void UpdatePlayScreen();
void DrawPlayScreen();


#endif // PLAY_SCREEN_H_
