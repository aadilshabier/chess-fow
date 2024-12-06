#include "loadingscreen.h"

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "gamestate.h"
#include "playscreen.h"

// From main.c
extern const int screenWidth;
extern const int screenHeight;
extern enum GameState state;
extern const Color bgColor;

static const int buttonWidth = 300;
static const int buttonHeight = 100;
static bool play = false;
static bool fogOfWar = false;
static bool multiplayer = false;

void InitLoadingScreen()
{
	GuiSetStyle(DEFAULT, TEXT_SIZE, 36);
	play = false;
	bool fogOfWar = false;
	bool multiplayer = false;
}


void UpdateLoadingScreen()
{
	if (play) {
		state = GAME_STATE_PLAY;
		InitPlayScreen();
	}
}

void DrawLoadingScreen()
{
	ClearBackground(bgColor);

	const char *titleText = "Chess!";
	const int titleSize = 50;
	int textWidth = MeasureText(titleText, titleSize);

	DrawText(titleText, (screenWidth-textWidth)/2, (screenHeight-3*buttonHeight-titleSize)/2, titleSize, BLACK);
	if (GuiButton((Rectangle){(screenWidth-buttonWidth)/2, (screenHeight-buttonHeight)/2, buttonWidth, buttonHeight}, "Play!"))
	    play = true;
	GuiCheckBox((Rectangle){(screenWidth-buttonWidth)/2, (screenHeight+buttonWidth/2)/2, buttonWidth/4, buttonHeight/2}, "Fog of War", &fogOfWar);
	GuiCheckBox((Rectangle){(screenWidth-buttonWidth)/2, (screenHeight+buttonWidth)/2, buttonWidth/4, buttonHeight/2}, "Multiplayer", &multiplayer);
}

