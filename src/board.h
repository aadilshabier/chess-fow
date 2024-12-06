#ifndef BOARD_H_
#define BOARD_H_

#include "raylib.h"

typedef enum Player {
	PLAYER_NONE,
	PLAYER_WHITE,
	PLAYER_BLACK,
} Player;

typedef enum Piece {
	PIECE_NONE,
	PIECE_KING,
	PIECE_QUEEN,
	PIECE_BISHOP,
	PIECE_KNIGHT,
	PIECE_ROOK,
	PIECE_PAWN,
} Piece;

typedef struct Cell {
	Rectangle rect;
    enum Player player;
	enum Piece piece;
} Cell;

typedef struct Board {
	struct Cell cells[8][8];
} Board;

void InitDefaultBoardPieces(Board *board);

void PlacePiece(Board *board, Player player, Piece piece, int x, int y);

void DrawPiece(const Cell *cell);

#endif // BOARD_H_
