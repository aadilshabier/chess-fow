#ifndef COMMS_SERVER_H_
#define COMMS_SERVER_H_

#include "dyad.h"

#include "comms.h"
#include "piece.h"

typedef enum PlayerState {
	PLAYER_STATE_NONE,
	PLAYER_STATE_CONNECTED,
	PLAYER_STATE_PLAY_READY, // ready to play
	PLAYER_STATE_PLAYING,
} PlayerState;


typedef struct PlayerConn {
	PlayerState state;
    dyad_Stream *stream;
	Player player;
} PlayerConn;

typedef struct Match {
    PlayerConn players[2];
	int activePlayers;
	int movesPlayed;
} Match;

PlayerConn makePlayerConn(void);

void onData(dyad_Event *e);
void onAccept(dyad_Event *e);
void onError(dyad_Event *e);

#endif // COMMS_SERVER_H_
