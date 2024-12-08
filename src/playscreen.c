#include "playscreen.h"

#include <stddef.h>
#include "stdio.h"

#include "raymath.h"

#include "board.h"
#include "loadingscreen.h"

// From main.c
extern const int screenWidth;
extern const int screenHeight;
extern enum GameState state;
extern const Color bgColor;
extern int multiplayerMode;

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

static void SinglePlayerUpdatePlayScreen()
{
	if (!cellSelected && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mousePos = GetMousePosition();
		sourceCell = findPointCell(&board, mousePos);
		if (sourceCell->player != board.player || sourceCell->piece == PIECE_NONE) {
			sourceCell = NULL;
		}
		if (sourceCell) {
			cellSelected = true;
		}
	} else if (cellSelected && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mousePos = GetMousePosition();
	    Cell *targetCell = findPointCell(&board, mousePos);


		// cancel move
		if (sourceCell == targetCell) {
			cellSelected = false;
			sourceCell = NULL;
			return;
		}

		// not allowed to move to same color
		if (sourceCell->player == targetCell->player) {
			return;
		}

		MovePiece(sourceCell, targetCell);
		board.player = otherPlayer(board.player);

		cellSelected = false;
		sourceCell = NULL;
	}
}

static void MultiPlayerUpdatePlayScreen()
{
}

void UpdatePlayScreen()
{
	if (multiplayerMode == 0) {
		SinglePlayerUpdatePlayScreen();
	} else {
		MultiPlayerUpdatePlayScreen();
	}
}

void DrawPlayScreen()
{
	ClearBackground(bgColor);

	const Vector2 screenSize = {screenWidth, screenHeight};
	const Vector2 cellSizeV = {cellSize, cellSize};
	const Vector2 offset = Vector2Scale(Vector2Subtract(screenSize, Vector2Scale(cellSizeV, numCells)), 0.5f);

	// Draw curent player
	char playerText[16];
	if (board.player == PLAYER_WHITE) {
		snprintf(playerText, 5+1, "%s", "White");
	} else {
		snprintf(playerText, 5+1, "%s", "Black");
	}
	sprintf(playerText+5, "'s Turn");
	DrawText(playerText, offset.x, offset.y-cellSize, 30, BLACK);

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
