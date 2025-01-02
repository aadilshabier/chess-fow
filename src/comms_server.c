#include "comms_server.h"

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyad.h"

PlayerConn makePlayerConn(void)
{
	PlayerConn conn = {
		.state = PLAYER_STATE_NONE,
		.stream = NULL,
		.player = PLAYER_NONE,
	};
	initMessageQueue(&conn.queue);
	return conn;
}

void onListen(dyad_Event *e) {
	printf("Server listening on localhost:%d\n", dyad_getPort(e->stream));
}

void onAccept(dyad_Event *e) {
	Match *match = e->udata;
	if (match->activePlayers >= 2) {
		const char *content = "ERROR: No space in the server!";
		fprintf(stderr, "%s\n", content);
		sendMessage(e->remote, MSG_ERROR, content);
		dyad_end(e->stream);
		return;
	};
	PlayerConn player = {
		.state = PLAYER_STATE_CONNECTED,
		.stream = e->remote,
		.player = PLAYER_NONE,
	};
	match->players[match->activePlayers] = player;
	match->activePlayers++;
	printf("Player number %d connected: %d\n", match->activePlayers, dyad_getSocket(player.stream));
	// messages might be small
	dyad_setNoDelay(player.stream, true);
	dyad_addListener(e->remote, DYAD_EVENT_DATA, onData, match);
}

static int getPlayerId(const Match *match, dyad_Stream *stream)
{
	for (int i=0; i<match->activePlayers; i++) {
		if (match->players[i].stream == stream)
			return i;
	}
	return -1;
}

void onData(dyad_Event *e)
{
	dyad_Stream *stream = e->stream;
	Match *match = e->udata;
	int playerIdx = getPlayerId(match, stream);
	if (playerIdx < 0) {
		fprintf(stderr, "ERROR: fd = %d is nout found\n", dyad_getSocket(stream));
		return;
	}

	PlayerConn *player = &match->players[playerIdx];
	// TODO: make sure that messages are read in order too!
	readMessagesToQueue(e->data, e->size, &player->queue);
}

void onError(dyad_Event *e) {
	fprintf(stderr, "ERROR: Server: %s\n", e->msg);
}


void onClose(dyad_Event *e)
{
	dyad_Stream *stream = e->stream;
	Match *match = e->udata;
	if (match->players[0].stream == stream) {
		memmove(&match->players[0], &match->players[1], sizeof(match->players[0]));
		match->activePlayers--;
	} else if (match->players[1].stream == stream) {
		match->activePlayers--;
	} else {
		return;
	}
}

