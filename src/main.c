#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define UNPACK_VEC2(v) (v).x,(v).y

const int screenWidth = 800;
const int screenHeight = 1000;

enum GameState {
	GAME_STATE_LOAD,
	GAME_STATE_PLAY,
} state;

// LOADING SCREEN
const Color bgColor = RAYWHITE;
const int buttonWidth = 300;
const int buttonHeight = 100;
bool play = false;
bool fogOfWar = false;
bool multiplayer = false;

void InitLoadingScreen();
void UpdateLoadingScreen();
void DrawLoadingScreen();
// ------------------------------

// GAME SCREEN
const char *spriteFile = "assets/1024px-Chess_Pieces_Sprite.svg.png";
const int cellSize = 80;
const int numCells = 8;
const int borderThickness = 10;
const Color whiteCellColor = WHITE;
/* const Color blackCellColor = BLACK; */
const Color blackCellColor = BROWN;
const Color borderColor = DARKBROWN;
Texture2D spriteTexture = {0};

void InitPlayScreen();
void UpdatePlayScreen();
void DrawPlayScreen();
// ------------------------------

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

void InitPlayScreen()
{
	Image spriteImg = LoadImage(spriteFile);
	ImageResize(&spriteImg, 6*cellSize, 2*cellSize);
	spriteTexture = LoadTextureFromImage(spriteImg);
	UnloadImage(spriteImg);
}

void UpdatePlayScreen()
{
}

void DrawPlayScreen()
{
	ClearBackground(bgColor);

	const Vector2 screenSize = {screenWidth, screenHeight};
	const Vector2 cellSizeV = {cellSize, cellSize};

	const Vector2 offset = Vector2Scale(Vector2Subtract(screenSize, Vector2Scale(cellSizeV, numCells)), 0.5f);

	// Draw Border
	DrawRectangleLinesEx((Rectangle){offset.x - borderThickness,offset.y - borderThickness,
									 numCells*cellSize + 2*borderThickness, numCells*cellSize + 2*borderThickness},
	    borderThickness, borderColor);

	// Draw grids
	for (int i=0; i<numCells; i++) {
		for (int j=0; j<numCells; j++) {
			Color color = ((i + j) % 2) ? blackCellColor : whiteCellColor;
			Vector2 pos = Vector2Add(offset, Vector2Multiply(cellSizeV, (Vector2){i,j}));
			DrawRectangleV(pos, cellSizeV, color);
			if (j < 4)
				DrawTextureRec(spriteTexture, (Rectangle){0, 0, cellSize, cellSize}, pos, WHITE);
			else
				DrawTextureRec(spriteTexture, (Rectangle){0, cellSize, cellSize, cellSize}, pos, WHITE);
		}
	}

	// Draw Pieces
	/* DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Color tint) */
}
