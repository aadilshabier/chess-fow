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
	enum Player player;
	struct Cell cells[8][8];
} Board;

void InitDefaultBoardPieces(Board *board);

void PlacePiece(Cell *cell, Player player, Piece piece);
void RemovePiece(Cell *cell);
void MovePiece(Cell *source, Cell *target);

void DrawPiece(const Cell *cell);

Cell *findPointCell(Board *board, Vector2 point);

#endif // BOARD_H_
