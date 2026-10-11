#include "comms_server.h"

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyad.h"

/* static int gPlayerId = 0; */

PlayerConn makePlayerConn(void) {
	PlayerConn conn = {
		.state = PLAYER_STATE_NONE,
		.stream = NULL,
		.player = PLAYER_NONE,
		/* .playerId = gPlayerId++, */
	};
	initMessageQueue(&conn.queue);
	return conn;
}

void onListen(dyad_Event *e) {
	printf("Server listening on localhost:%d\n", dyad_getPort(e->stream));
}

void onAccept(dyad_Event *e) {
	ServerGamesManager *gamesManager = e->udata;

	// TODO: read preference first before matching to player slot
	int gameIdx, playerIdx;
	PlayerConn *player = reservePlayerSlot(gamesManager, &gameIdx, &playerIdx);
	if (player == NULL) {
		const char *content = "ERROR: Could not find an available slot!";
		fputs(content, stderr);
		sendMessage(e->remote, MSG_ERROR, content);
		dyad_end(e->stream);
		return;
	}
	printf("Connected game %d, player %d.\n", gameIdx+1, playerIdx+1);
	*player = makePlayerConn();
	player->state = PLAYER_STATE_CONNECTED;
	player->stream = e->remote;

	activatePlayerConn(gamesManager, player);
}

void onData(dyad_Event *e)
{
	PlayerConn *player = e->udata;
	// TODO: make sure that messages are read in order too!
	readMessagesToQueue(e->data, e->size, &player->queue);
}

void onError(dyad_Event *e) {
	fprintf(stderr, "ERROR: Server: %s\n", e->msg);
}

void onServerClose(dyad_Event */*e*/)
{
	// Unimplemented
	return;
}

void onClientClose(dyad_Event *e) {
	ServerGamesManager *gamesManager = e->udata;
	for (int i = 0; i < MAX_GAMES; i++) {
		Game *game = &gamesManager->games[i];
		if (game->numPlayers == 0) continue;
		for (int j = 0; i < 2; j++) {
			PlayerConn *player = &game->players[j];
			if (player->stream != e->stream) continue;
			// clear player entry
			// TODO: maybe inform other player?
			player->state = PLAYER_STATE_NONE;
			player->stream = NULL;
			player->player = PLAYER_NONE;
			--game->numPlayers;
			/* .playerId = gPlayerId++, */
			return;
		}
	}
}

void activatePlayerConn(ServerGamesManager* gamesManager, PlayerConn *player) {
	// messages might be small
	dyad_setNoDelay(player->stream, true);
	dyad_addListener(player->stream, DYAD_EVENT_DATA, onData, player);
	dyad_addListener(player->stream, DYAD_EVENT_CLOSE, onClientClose, gamesManager);
}

PlayerConn *reservePlayerSlot(ServerGamesManager *gamesManager, int *gameIdx, int *playerIdx) {
	for (int i = 0; i < MAX_GAMES; i++) {
		Game *game = &gamesManager->games[i];
		if (game->numPlayers >= 2) continue;

		// set idx values, increment player count, and return
		if (gameIdx) *gameIdx = i;
		int playerIdx_ = game->numPlayers;
		if (playerIdx) *playerIdx = playerIdx_;

		// increment counters
		++game->numPlayers;
		++gamesManager->numPlayers;
		if (game->numPlayers == 1) {
			++gamesManager->numGames;
		}

		return &game->players[playerIdx_];
	}
	return NULL;
}
