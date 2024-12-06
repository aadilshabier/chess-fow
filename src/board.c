#include "board.h"

#include <stddef.h>

#include "playscreen.h"

extern Texture2D spriteTexture;

void InitDefaultBoardPieces(struct Board *board)
{
	// Set to None
	for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			board->cells[i][j].player = PLAYER_NONE;
			board->cells[i][j].piece = PIECE_NONE;
		}
	}

	// Color
	for (int i=0; i<8; i++) {
		board->cells[0][i].player = PLAYER_BLACK;
		board->cells[1][i].player = PLAYER_BLACK;
		board->cells[6][i].player = PLAYER_WHITE;
		board->cells[7][i].player = PLAYER_WHITE;
	}

	// Pawns
	for (int i=0; i<8; i++) {
		board->cells[1][i].piece = PIECE_PAWN;
		board->cells[6][i].piece = PIECE_PAWN;
	}

	// Rooks
	board->cells[0][0].piece = PIECE_ROOK;
	board->cells[0][7].piece = PIECE_ROOK;
	board->cells[7][0].piece = PIECE_ROOK;
	board->cells[7][7].piece = PIECE_ROOK;

	// Knights
	board->cells[0][1].piece = PIECE_KNIGHT;
	board->cells[0][6].piece = PIECE_KNIGHT;
	board->cells[7][1].piece = PIECE_KNIGHT;
	board->cells[7][6].piece = PIECE_KNIGHT;

	// Bishops
	board->cells[0][2].piece = PIECE_BISHOP;
	board->cells[0][5].piece = PIECE_BISHOP;
	board->cells[7][2].piece = PIECE_BISHOP;
	board->cells[7][5].piece = PIECE_BISHOP;

	// King & Queen
	board->cells[0][3].piece = PIECE_QUEEN;
	board->cells[0][4].piece = PIECE_KING;
	board->cells[7][3].piece = PIECE_QUEEN;
	board->cells[7][4].piece = PIECE_KING;
}

void DrawPiece(const Cell *cell)
{
	if (cell->player == PLAYER_NONE || cell->piece == PIECE_NONE) {
		return;
	}

	int j = cell->player-1;
	int i = cell->piece-1;

	Rectangle source = {i*cellSize, j*cellSize, cellSize, cellSize};
	Vector2 pos = {cell->rect.x, cell->rect.y};
	DrawTextureRec(spriteTexture, source, pos, WHITE);
}

Cell *findPointCell(Board *board, Vector2 point)
{
	Cell *result = NULL;
	for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			if (CheckCollisionPointRec(point, board->cells[i][j].rect)) {
				result = &board->cells[i][j];
				goto found;
			}
		}
	}
 found:
	return result;
}

void PlacePiece(Cell *cell, Player player, Piece piece)
{
	cell->player = player;
	cell->piece = piece;
}

void RemovePiece(Cell *cell)
{
	PlacePiece(cell, PLAYER_NONE, PIECE_NONE);
}

void MovePiece(Cell *source, Cell *target)
{
	PlacePiece(target, source->player, source->piece);
	RemovePiece(source);
}
