#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

const int screenWidth = 1000;
const int screenHeight = 800;

// LOADING SCREEN
enum GameState {
	GAME_STATE_LOAD_SCREEN,
	GAME_STATE_PLAY,
} state;

const int buttonWidth = 300;
const int buttonHeight = 100;
bool play = false;
bool fogOfWar = false;
bool multiplayer = false;

void InitLoadingScreen();
void UpdateLoadingScreen();
void DrawLoadingScreen();
// ------------------------------

int main(void)
{
    // Initialization
    InitWindow(screenWidth, screenHeight, "Chess");
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

	state = GAME_STATE_LOAD_SCREEN;
	InitLoadingScreen();
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
		switch (state) {
		case GAME_STATE_LOAD_SCREEN:
			UpdateLoadingScreen(); break;
		default:
			return 1;
		}
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
		{			
            ClearBackground(RAYWHITE);

			switch (state) {
			case GAME_STATE_LOAD_SCREEN:
				DrawLoadingScreen(); break;
			default:
				return 1;
			}
        }
		EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void InitLoadingScreen()
{
	GuiSetStyle(DEFAULT, TEXT_SIZE, 36);
	play = false;
}


void UpdateLoadingScreen()
{
	if (play) {
		// state = GAME_STATE_PLAY;
		// InitGameState();
	}
}

void DrawLoadingScreen()
{
	const char *titleText = "Chess!";
	const int titleSize = 50;
	int textWidth = MeasureText(titleText, titleSize);
	DrawText(titleText, (screenWidth-textWidth)/2, (screenHeight-3*buttonHeight-titleSize)/2, titleSize, BLACK);
	if (GuiButton((Rectangle){(screenWidth-buttonWidth)/2, (screenHeight-buttonHeight)/2, buttonWidth, buttonHeight}, "Play!"))
	    play = true;
	GuiCheckBox((Rectangle){(screenWidth-buttonWidth)/2, (screenHeight+buttonWidth/2)/2, buttonWidth/4, buttonHeight/2}, "Fog of War", &fogOfWar);
	GuiCheckBox((Rectangle){(screenWidth-buttonWidth)/2, (screenHeight+buttonWidth)/2, buttonWidth/4, buttonHeight/2}, "Multiplayer", &multiplayer);
}
