#ifndef PLAY_SCREEN_H_
#define PLAY_SCREEN_H_

#include "raylib.h"

extern Texture2D spriteTexture;
extern const int cellSize;

void InitPlayScreen();
void UpdatePlayScreen();
void DrawPlayScreen();


#endif // PLAY_SCREEN_H_
