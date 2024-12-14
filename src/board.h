#ifndef BOARD_H_
#define BOARD_H_

#include "piece.h"
#include "raylib.h"

typedef struct Cell {
	Rectangle rect;
    enum Player player;
	enum Piece piece;
	bool isSelected;
	bool isCandidate;
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

Player otherPlayer(Player player);

void cellToIdx(const Board *board, const Cell *cell, int *x, int *y);

void getCandidates(Board *board, int x, int y, Cell *candidates[], int *num_candidates);
void markCandidates(Board *board, int x, int y);
void unmarkCandidates(Board *board);

#endif // BOARD_H_
