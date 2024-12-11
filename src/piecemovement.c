#include "piecemovement.h"
#include "board.h"

void (*candidateGetters[])(Board *board, int x, int y, Cell *candidates[], int *num_candidates) = {
	[PIECE_PAWN] = &getPawnCandidates,
};

bool isValidCell(int x, int y)
{
	return x >= 0 && y >= 0 && x < 8 && y < 8;
}

void getPawnCandidates(Board *board, int x, int y, Cell *candidates[], int *num_candidates)
{
	Player player = board->cells[y][x].player;
	int direction = (player == PLAYER_WHITE) ? -1 : 1;
	// TODO: for loop instead of this
	// check 1 step forward
	int y_forward = y+direction;
	if (!isValidCell(x, y_forward) || board->cells[y_forward][x].player != PLAYER_NONE) {
		return;
	}
	candidates[*num_candidates] = &board->cells[y_forward][x];
	(*num_candidates)++;

	// check 2 steps forward if pawn hasn't moved yet
	y_forward = y+2*direction;
	int y_base = (player == PLAYER_WHITE) ? 6 : 1;
	if (y != y_base || !isValidCell(x, y_forward) || board->cells[y_forward][x].player != PLAYER_NONE) {
		return;
	}
	candidates[*num_candidates] = &board->cells[y_forward][x];
	(*num_candidates)++;
}
