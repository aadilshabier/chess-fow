#ifndef COMMS_SERVER_H_
#define COMMS_SERVER_H_

#include "dyad.h"

#include "comms.h"
#include "piece.h"

typedef struct PlayerConn {
	PlayerState state;
    dyad_Stream *stream;
	MessageQueue queue;
	Player player;
	/* int playerId; */
} PlayerConn;

// Game handling logic
// TODO: potentially move out
typedef struct Game {
    PlayerConn players[2];
	int numPlayers;
	int movesPlayed;
} Game;

// TODO: potentially move out

#define MAX_GAMES 20
// Manages the games on this server, is responsible for matching games
typedef struct ServerGamesManager {
	Game games[MAX_GAMES];
	int numGames;
	int numPlayers;
} ServerGamesManager;

// Add listener to player->stream which drains messages into player->queue
// Also add exit handling when client closes the connection.
void activatePlayerConn(ServerGamesManager* gamesManager, PlayerConn *player);

// Player - Game matching logic

// Reserves, and returns the next available playerConn slot, or NULL if there was none available
// Sets gameIdx and playerIdx accordingly
PlayerConn *reservePlayerSlot(ServerGamesManager *gamesManager, int *gameIdx, int *playerIdx);

PlayerConn makePlayerConn(void);

void onData(dyad_Event *e);
void onListen(dyad_Event *e);
void onAccept(dyad_Event *e);
void onError(dyad_Event *e);
void onServerClose(dyad_Event *e);
void onClientClose(dyad_Event *e);

#endif // COMMS_SERVER_H_
