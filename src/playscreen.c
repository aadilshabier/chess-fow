#include "playscreen.h"

#include "raymath.h"

#include "board.h"

// From main.c
extern const int screenWidth;
extern const int screenHeight;
extern enum GameState state;
extern const Color bgColor;

Texture2D spriteTexture = {0};
const int cellSize = 80;

static const char *spriteFile = "assets/1024px-Chess_Pieces_Sprite.svg.png";
static const int numCells = 8;
static const int borderThickness = 10;
static const Color whiteCellColor = WHITE;
static const Color blackCellColor = BROWN;
static const Color borderColor = DARKBROWN;

static Board board;


void InitPlayScreen()
{
	Image spriteImg = LoadImage(spriteFile);
	ImageResize(&spriteImg, 6*cellSize, 2*cellSize);
	spriteTexture = LoadTextureFromImage(spriteImg);
	UnloadImage(spriteImg);

	// Initialize board
	InitDefaultBoardPieces(&board);
	const Vector2 screenSize = {screenWidth, screenHeight};
	const Vector2 cellSizeV = {cellSize, cellSize};
	const Vector2 offset = Vector2Scale(Vector2Subtract(screenSize, Vector2Scale(cellSizeV, numCells)), 0.5f);
	for (int i=0; i<numCells; i++) {
		for (int j=0; j<numCells; j++) {
			Vector2 pos = Vector2Add(offset, Vector2Multiply(cellSizeV, (Vector2){j,i}));
			board.cells[i][j].rect = (Rectangle){pos.x, pos.y, cellSize, cellSize};
		}
	}
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
			DrawRectangleRec(board.cells[i][j].rect, color);
			DrawPiece(&board.cells[i][j]);
		}
	}

	// Draw Pieces
	/* DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Color tint) */
}
