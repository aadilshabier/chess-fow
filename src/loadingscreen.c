#include "loadingscreen.h"

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "board.h"
#include "gamestate.h"
#include "playscreen.h"

// From main.c
extern const int screenWidth;
extern const int screenHeight;
extern const Color bgColor;

extern int multiplayerMode;

int multiplayerMode = 0;
PlayerToggleState playerToggle = PLAYER_TOGGLE_RANDOM;

static const int buttonWidth = 300;
static const int buttonHeight = 100;
static bool play = false;
static bool fogOfWar = false;

GameState _LOADINGSTATEOBJ = {
	.init = &InitLoadingScreen,
	.update = &UpdateLoadingScreen,
	.draw = &DrawLoadingScreen,
	.exit = NULL,
};

void InitLoadingScreen()
{
	GuiSetStyle(DEFAULT, TEXT_SIZE, 36);
	play = false;
    fogOfWar = false;
    multiplayerMode = 0;
	playerToggle = PLAYER_TOGGLE_RANDOM;
}


void UpdateLoadingScreen()
{
	if (!play) {
		return;
	}
	if (!multiplayerMode) {
		Player player = PLAYER_WHITE;
		replaceGameState(GAME_STATE_PLAY, &player);
	} else {
		replaceGameState(GAME_STATE_MULTI_MENU, &playerToggle);
	}
}

void DrawLoadingScreen()
{
	ClearBackground(bgColor);

	const char *titleText = "Chess!";
	const int titleSize = 50;
	int textWidth = MeasureText(titleText, titleSize);

	DrawText(titleText, (screenWidth-textWidth)/2, (screenHeight-3*buttonHeight-titleSize)/2, titleSize, BLACK);
	play = GuiButton((Rectangle){(screenWidth-buttonWidth)/2, (screenHeight-buttonHeight)/2, buttonWidth, buttonHeight}, "Play!");
	GuiCheckBox((Rectangle){(screenWidth-buttonWidth)/2, (screenHeight+buttonWidth/2)/2, buttonWidth/4, buttonHeight/2}, "Fog of War", &fogOfWar);
	GuiToggleSlider((Rectangle){(screenWidth-buttonWidth)/2, (screenHeight+buttonWidth)/2, buttonWidth, buttonHeight/2}, "Local;P2P", &multiplayerMode);
	if (multiplayerMode == 1) {
		GuiToggleSlider((Rectangle){(screenWidth-buttonWidth)/2, (screenHeight+3*buttonWidth/2)/2, buttonWidth, buttonHeight/2},
						"White;Random;Black", (int*)&playerToggle);
	} else {
		playerToggle = PLAYER_TOGGLE_RANDOM;
	}
}
