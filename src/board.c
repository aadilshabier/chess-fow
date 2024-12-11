#include "board.h"

#include <stddef.h>

#include "gamestate.h"
#include "playscreen.h"

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
	if (source != target)
		RemovePiece(source);
}

Player otherPlayer(Player player)
{
	switch (player) {
	case PLAYER_WHITE: return PLAYER_BLACK;
	case PLAYER_BLACK: return PLAYER_WHITE;
	}
}

void cellToIdx(const Board *board, const Cell *cell, int *x, int *y)
{
	const Cell* base = &board->cells[0][0];
	size_t position = (cell - base);
	*x = position % 8;
	*y = position / 8;
}

static bool isValidCell(int x, int y)
{
	return x >= 0 && y >= 0 && x < 8 && y < 8;
}

static void getPawnCandidates(Board *board, int x, int y, Cell *candidates[], int *num_candidates)
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

void getCandidates(Board *board, int x, int y, Cell *candidates[], int *num_candidates)
{
	*num_candidates = 0;

    Piece piece = board->cells[y][x].piece;

	switch (piece) {
	case PIECE_PAWN: getPawnCandidates(board, x, y, candidates, num_candidates); break;
	default: TraceLog(LOG_ERROR, "Not implemented for piece: %d", piece); break;
	}
}

void markCandidates(Board *board, int x, int y) {
	Cell *candidates[64];
    int num_candidates;
	getCandidates(board, x, y, candidates, &num_candidates);
	TraceLog(LOG_INFO, "Marking candidates for (%d, %d)", x, y);

	for (size_t i=0; i<num_candidates; i++) {
		int x_, y_;
		cellToIdx(board, candidates[i], &x_, &y_);
		TraceLog(LOG_INFO, "Candidate %i: (%d,%d)", i+1, x_, y_);
		candidates[i]->isCandidate = true;
	}
}

void unmarkCandidates(Board *board)
{
	for (int y=0; y<8; y++) {
		for (int x=0; x<8; x++) {
			board->cells[y][x].isCandidate = false;
		}
	}
}
