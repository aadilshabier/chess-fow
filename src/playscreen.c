#include "playscreen.h"

#include <stddef.h>
#include <string.h>

#include "raymath.h"

#include "board.h"
#include "comms_client.h"
#include "loadingscreen.h"
#include "multiscreen.h"

// From game.c
extern const int screenWidth;
extern const int screenHeight;
extern enum GameStateType state;
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
static const Color candidateColor = (Color){250, 210, 0, 120};

Board board;
static Cell *sourceCell = NULL;

GameState _PLAYSTATEOBJ = {
	.init = &InitPlayScreen,
	.update = &UpdatePlayScreen,
	.draw = &DrawPlayScreen,
	.exit = NULL,
};

void InitPlayScreen(void *playerPtr)
{
	Image spriteImg = LoadImage(spriteFile);
	ImageResize(&spriteImg, 6*cellSize, 2*cellSize);
	spriteTexture = LoadTextureFromImage(spriteImg);
	UnloadImage(spriteImg);

	// Initialize board
	board.player = PLAYER_WHITE;
	clientPlayer = *(Player*)playerPtr;
	InitDefaultBoardPieces(&board);
	const Vector2 screenSize = {screenWidth, screenHeight};
	const Vector2 cellSizeV = {cellSize, cellSize};
	const Vector2 offset = Vector2Scale(Vector2Subtract(screenSize, Vector2Scale(cellSizeV, numCells)), 0.5f);
	for (int i=0; i<numCells; i++) {
		for (int j=0; j<numCells; j++) {
			Vector2 pos = Vector2Add(offset, Vector2Multiply(cellSizeV, (Vector2){j,i}));
			if (!multiplayerMode || clientPlayer != PLAYER_BLACK) {
				board.cells[i][j].rect = (Rectangle){pos.x, pos.y, cellSize, cellSize};
			} else {
				board.cells[8-i-1][8-j-1].rect = (Rectangle){pos.x, pos.y, cellSize, cellSize};
			}
		}
	}

	sourceCell = NULL;
}

static void LocalPlayerUpdatePlayScreen()
{
	if (!sourceCell && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mousePos = GetMousePosition();
		sourceCell = findPointCell(&board, mousePos);
		if (!sourceCell || sourceCell->player != board.player || sourceCell->piece == PIECE_NONE) {
			sourceCell = NULL;
		}
		if (sourceCell) {
			sourceCell->isSelected = true;
			int x, y;
			cellToIdx(&board, sourceCell, &x, &y);
			markCandidates(&board, x, y);
		}
	} else if (sourceCell && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mousePos = GetMousePosition();
	    Cell *targetCell = findPointCell(&board, mousePos);

		// cancel move
		if (sourceCell == targetCell) {
			sourceCell->isSelected = false;
			sourceCell = NULL;
			unmarkCandidates(&board);
			return;
		}

		// check if candidate cell
		if (targetCell->isCandidate) {
			MovePiece(sourceCell, targetCell);
			board.player = otherPlayer(board.player);
			sourceCell->isSelected = false;
			sourceCell = NULL;
			unmarkCandidates(&board);
		}
	}
}

static void MultiPlayerUpdatePlayScreen()
{
	dyad_update();
	if (board.player != clientPlayer)
		return;
	if (!sourceCell && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mousePos = GetMousePosition();
		sourceCell = findPointCell(&board, mousePos);
		if (!sourceCell || sourceCell->player != clientPlayer || sourceCell->piece == PIECE_NONE) {
			sourceCell = NULL;
		}
		if (sourceCell) {
			sourceCell->isSelected = true;
			int x, y;
			cellToIdx(&board, sourceCell, &x, &y);
			markCandidates(&board, x, y);
		}
	} else if (sourceCell && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mousePos = GetMousePosition();
	    Cell *targetCell = findPointCell(&board, mousePos);

		// cancel move
		if (sourceCell == targetCell) {
			sourceCell->isSelected = false;
			sourceCell = NULL;
			unmarkCandidates(&board);
			return;
		}

		// check if candidate cell
		if (targetCell->isCandidate) {
			MovePiece(sourceCell, targetCell);

			// send move
			Move move;
			int x, y;
			cellToIdx(&board, sourceCell, &x, &y);
			move.source.x = x;
			move.source.y = y;
			cellToIdx(&board, targetCell, &x, &y);
			move.target.x = x;
			move.target.y = y;

		    sendMessage(clientStream, MSG_SEND_MOVE, &move);

			board.player = otherPlayer(board.player);
			sourceCell->isSelected = false;
			sourceCell = NULL;
			unmarkCandidates(&board);
		}
	}
}

void UpdatePlayScreen()
{
	if (multiplayerMode == 0) {
	    LocalPlayerUpdatePlayScreen();
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
	char *s = stpcpy(playerText, (board.player == PLAYER_WHITE) ? "White":"Black");
	stpcpy(s, "'s Turn");
	DrawText(playerText, offset.x, offset.y-cellSize, 30, BLACK);

	if (multiplayerMode) {
		s = stpcpy(playerText, "You're ");
		stpcpy(s, (clientPlayer == PLAYER_WHITE) ? "White":"Black");
		DrawText(playerText, offset.x+5*cellSize, offset.y-2*cellSize, 30, BLACK);
	}


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
			if (board.cells[i][j].isCandidate) {
				DrawRectangleRec(board.cells[i][j].rect, candidateColor);
			} else if (board.cells[i][j].isSelected) {
				DrawRectangleRec(board.cells[i][j].rect, selectedColor);
			}
		}
	}
}
