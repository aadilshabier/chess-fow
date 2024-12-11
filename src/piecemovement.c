#include "piecemovement.h"
#include "board.h"

void (*candidateGetters[])(Board *board, int x, int y, Cell *candidates[], int *num_candidates) = {
	[PIECE_KNIGHT] = &getKnightCandidates,
	[PIECE_PAWN] = &getPawnCandidates,
};

bool isValidCell(int x, int y)
{
	return x >= 0 && y >= 0 && x < 8 && y < 8;
}

static void addCandidate(Cell *candidates[], int *num_candidates, Cell *newCandidate)
{
	candidates[*num_candidates] = newCandidate;
	(*num_candidates)++;
}

void getPawnCandidates(Board *board, int x, int y, Cell *candidates[], int *num_candidates)
{
	Player player = board->cells[y][x].player;
	int direction = (player == PLAYER_WHITE) ? -1 : 1;
	int y_forward = y+direction;

	// check diagonals
	// TODO: implement En passant
	for (int x_diag=x-1; x_diag<=x+1; x_diag+=2) {
		if (isValidCell(x_diag, y_forward) && board->cells[y_forward][x_diag].player == otherPlayer(player)) {
			addCandidate(candidates, num_candidates, &board->cells[y_forward][x_diag]);
		}
	}

	// check 1 step forward
	if (!isValidCell(x, y_forward) || board->cells[y_forward][x].player != PLAYER_NONE) {
		return;
	}
	addCandidate(candidates, num_candidates, &board->cells[y_forward][x]);

	// check 2 steps forward if pawn hasn't moved yet
	y_forward = y+2*direction;
	int y_base = (player == PLAYER_WHITE) ? 6 : 1;
	if (y != y_base || !isValidCell(x, y_forward) || board->cells[y_forward][x].player != PLAYER_NONE) {
		return;
	}
	addCandidate(candidates, num_candidates, &board->cells[y_forward][x]);
}

void getKnightCandidates(Board *board, int x, int y, Cell *candidates[], int *num_candidates) {
	Player player = board->cells[y][x].player;
	struct {int x, y; } offset[8] = {{-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}};
	for (int i=0; i < sizeof(offset)/sizeof(offset[0]); i++) {
		int x_ = x + offset[i].x;
		int y_ = y + offset[i].y;
		if (isValidCell(x_, y_) && board->cells[y_][x_].player != player) {
			addCandidate(candidates, num_candidates, &board->cells[y_][x_]);
		}
	}
}
