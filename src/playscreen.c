#include "playscreen.h"

#include <stddef.h>

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
static const Color selectedColor = (Color){135, 60, 190, 120};

static Board board;
static bool cellSelected = false;
static Cell *sourceCell = NULL;

void InitPlayScreen(Player player)
{
	Image spriteImg = LoadImage(spriteFile);
	ImageResize(&spriteImg, 6*cellSize, 2*cellSize);
	spriteTexture = LoadTextureFromImage(spriteImg);
	UnloadImage(spriteImg);

	// Initialize board
	board.player = player;
	InitDefaultBoardPieces(&board);
	const Vector2 screenSize = {screenWidth, screenHeight};
	const Vector2 cellSizeV = {cellSize, cellSize};
	const Vector2 offset = Vector2Scale(Vector2Subtract(screenSize, Vector2Scale(cellSizeV, numCells)), 0.5f);
	for (int i=0; i<numCells; i++) {
		for (int j=0; j<numCells; j++) {
			Vector2 pos = Vector2Add(offset, Vector2Multiply(cellSizeV, (Vector2){j,i}));
			if (board.player == PLAYER_WHITE) {
				board.cells[i][j].rect = (Rectangle){pos.x, pos.y, cellSize, cellSize};
			} else if (board.player == PLAYER_BLACK) {
				board.cells[8-i-1][8-j-1].rect = (Rectangle){pos.x, pos.y, cellSize, cellSize};
			}
		}
	}

	cellSelected = false;
	sourceCell = NULL;
}

void UpdatePlayScreen()
{
	if (!cellSelected && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mousePos = GetMousePosition();
		sourceCell = findPointCell(&board, mousePos);
		if (sourceCell->player == PLAYER_NONE || sourceCell->piece == PIECE_NONE) {
			sourceCell = NULL;
		}
		if (sourceCell) {
			cellSelected = true;
		}
	} else if (cellSelected && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mousePos = GetMousePosition();
	    Cell *targetCell = findPointCell(&board, mousePos);

		MovePiece(sourceCell, targetCell);

		cellSelected = false;
		sourceCell = NULL;
	}
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
			// tint if selected
			if (sourceCell == &board.cells[i][j]) {
				DrawRectangleRec(board.cells[i][j].rect, selectedColor);
			}
		}
	}
}
