#include "raylib.h"

#include <stddef.h>

#include "gamestate.h"
#include "playscreen.h"
#include "loadingscreen.h"
#include "multiscreen.h"

const Color bgColor = RAYWHITE;

const int screenWidth = 800;
const int screenHeight = 1000;

GameState *gameStates[NUM_GAME_STATES] = {
	[GAME_STATE_LOAD] = &_LOADINGSTATEOBJ,
	[GAME_STATE_PLAY] = &_PLAYSTATEOBJ,
	[GAME_STATE_MULTI_MENU] = &_MULTISTATEOBJ,
};
GameState *currentState = NULL;

int main(void)
{
    // Initialization
    InitWindow(screenWidth, screenHeight, "Chess");
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

	replaceGameState(GAME_STATE_LOAD, NULL);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
		if (currentState->update) {
			currentState->update();
		}
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
		if (currentState->draw) {
			currentState->draw();
		}
		EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
	UnloadTexture(spriteTexture);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void replaceGameState(GameStateType newState, void *arg)
{
	if (currentState && currentState->exit) {
		currentState->exit();
	}
	currentState = gameStates[newState];
	if (currentState->init) {
		currentState->init(arg);
	}
}
