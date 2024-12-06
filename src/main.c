#include "raylib.h"

#include "gamestate.h"
#include "playscreen.h"
#include "loadingscreen.h"

const Color bgColor = RAYWHITE;

const int screenWidth = 800;
const int screenHeight = 1000;

enum GameState state;

int main(void)
{
    // Initialization
    InitWindow(screenWidth, screenHeight, "Chess");
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

	/* state = GAME_STATE_LOAD; */
	/* InitLoadingScreen(); */

	state = GAME_STATE_PLAY;
	InitPlayScreen();
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
		switch (state) {
		case GAME_STATE_LOAD:
			UpdateLoadingScreen(); break;
		case GAME_STATE_PLAY:
		    UpdatePlayScreen(); break;
		default:
			TraceLog(LOG_ERROR, "State %d not handled for updating!", state);
			return 1;
		}
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
		{			
			switch (state) {
			case GAME_STATE_LOAD:
				DrawLoadingScreen(); break;
			case GAME_STATE_PLAY:
				DrawPlayScreen(); break;
			default:
				TraceLog(LOG_ERROR, "State %d not handled for drawing!", state);
				return 1;
			}
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
