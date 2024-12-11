#include "piecemovement.h"
#include "board.h"

void (*candidateGetters[])(Board *board, int x, int y, Cell *candidates[], int *num_candidates) = {
	[PIECE_KNIGHT] = &getKnightCandidates,
	[PIECE_PAWN] = &getPawnCandidates,
	[PIECE_BISHOP] = &getBishopCandidates,
	[PIECE_ROOK] = &getRookCandidates,
	[PIECE_QUEEN] = &getQueenCandidates,
	[PIECE_KING] = &getKingCandidates,
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
	*num_candidates = 0;
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
	*num_candidates = 0;
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

/* Move towards the offset and add candidates till we reach a block. A block is:
 * 1. End of board
 * 2. Our own piece
 * 3. Opponent piece (include this cell)
 */
static void addCandidatesTillEnd(Board *board, int x, int y, Cell *candidates[], int *num_candidates, int offsetX, int offsetY)
{
	Player player = board->cells[y][x].player;
	while (true) {
		x += offsetX;
		y += offsetY;

		if (!isValidCell(x, y) || board->cells[y][x].player == player) {
			break;
		}
		addCandidate(candidates, num_candidates, &board->cells[y][x]);
		if (board->cells[y][x].player == otherPlayer(player)) {
			break;
		}
	}
}

void getBishopCandidates(Board *board, int x, int y, Cell *candidates[], int *num_candidates)
{
	*num_candidates = 0;
	struct {int x, y; } offsets[4] = {{-1,1}, {1, 1}, {1, -1}, {-1, -1}};

	for (int i=0; i<sizeof(offsets)/sizeof(offsets[0]); i++) {
		addCandidatesTillEnd(board, x, y, candidates, num_candidates, offsets[i].x, offsets[i].y);
	}
}

void getRookCandidates(Board *board, int x, int y, Cell *candidates[], int *num_candidates)
{
	*num_candidates = 0;
	struct {int x, y; } offsets[4] = {{-1,0}, {1, 0}, {0, 1}, {0, -1}};

	for (int i=0; i<sizeof(offsets)/sizeof(offsets[0]); i++) {
		addCandidatesTillEnd(board, x, y, candidates, num_candidates, offsets[i].x, offsets[i].y);
	}
}

void getQueenCandidates(Board *board, int x, int y, Cell *candidates[], int *num_candidates)
{
	*num_candidates = 0;
	struct {int x, y; } offsets[8] = {{-1,1}, {1, 1}, {1, -1}, {-1, -1}, {-1,0}, {1, 0}, {0, 1}, {0, -1}};

	for (int i=0; i<sizeof(offsets)/sizeof(offsets[0]); i++) {
		addCandidatesTillEnd(board, x, y, candidates, num_candidates, offsets[i].x, offsets[i].y);
	}
}

void getKingCandidates(Board *board, int x, int y, Cell *candidates[], int *num_candidates)
{
	*num_candidates = 0;
	Player player = board->cells[y][x].player;
	struct {int x, y; } offsets[8] = {{-1,1}, {1, 1}, {1, -1}, {-1, -1}, {-1,0}, {1, 0}, {0, 1}, {0, -1}};

	// TODO: check for checks and implement castling
	for (int i=0; i<sizeof(offsets)/sizeof(offsets[0]); i++) {
		int x_ = x + offsets[i].x;
		int y_ = y + offsets[i].y;
		if (isValidCell(x_, y_) && board->cells[y_][x_].player != player) {
			addCandidate(candidates, num_candidates, &board->cells[y_][x_]);
		}
	}
}
