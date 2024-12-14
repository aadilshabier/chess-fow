#ifndef PIECE_H_
#define PIECE_H_

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
	NUM_PIECES,
} Piece;

#endif // PIECE_H_
