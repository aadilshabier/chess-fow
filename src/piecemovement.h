#ifndef PIECE_MOVEMENT_H_
#define PIECE_MOVEMENT_H_

#include "board.h"

extern void (*candidateGetters[NUM_PIECES])(Board *board, int x, int y, Cell *candidates[], int *num_candidates);

bool isValidCell(int x, int y);

void getPawnCandidates(Board *board, int x, int y, Cell *candidates[], int *num_candidates);

#endif // PIECE_MOVEMENT_H_
