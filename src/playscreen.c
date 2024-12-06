#include "playscreen.h"

#include "raymath.h"

// From main.c
extern const int screenWidth;
extern const int screenHeight;
extern enum GameState state;
extern const Color bgColor;

static const char *spriteFile = "assets/1024px-Chess_Pieces_Sprite.svg.png";
static const int cellSize = 80;
static const int numCells = 8;
static const int borderThickness = 10;
static const Color whiteCellColor = WHITE;
static const Color blackCellColor = BROWN;
static const Color borderColor = DARKBROWN;

Texture2D spriteTexture = {0};

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
