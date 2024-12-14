#include "comms_server.h"

#include <stdbool.h>
#include <stdio.h>

#include "dyad.h"

PlayerConn makePlayerConn(void)
{
	PlayerConn conn = {
		.state = PLAYER_STATE_NONE,
		.stream = NULL,
		.player = PLAYER_NONE,
	};
	return conn;
}

void onAccept(dyad_Event *e) {
	Match *match = e->udata;
	if (match->activePlayers >= 2) {
		const char *content = "ERROR: No space in the server!";
		fprintf(stderr, "%s\n", content);
		Message *message = makeMessage(MSG_PLAY_ERROR, content);
		char buffer[1024];
		serializeMessage(message, buffer);
		dyad_write(e->remote, &message, sizeof(message));
		freeMessage(message);
		return;
	};
	PlayerConn player = {
		.state = PLAYER_STATE_CONNECTED,
		.stream = e->remote,
		.player = PLAYER_NONE,
	};
	match->players[match->activePlayers] = player;
	match->activePlayers++;
	printf("Player connected: %d\n", dyad_getSocket(player.stream));
	// messages might be small
	dyad_setNoDelay(player.stream, true);
	dyad_addListener(e->remote, DYAD_EVENT_DATA, onData, match);
}

static bool expect(const char *name, int expected, int value) {
	if (expected != value) {
		fprintf(stderr, "Expected %s(%d), got %d\n", name, expected, value);
		return true;
	}
	return false;
}

void onData(dyad_Event *e)
{
	dyad_Stream *stream = e->stream;
	Match *match = e->udata;
	int playerIdx;
	if (match->players[0].stream == stream) playerIdx = 0;
	else if (match->players[1].stream == stream) playerIdx = 1;
	else {
		fprintf(stderr, "ERROR: playerIdx=%d is invalid\n", playerIdx);
		return;
	}

	uint32_t length = *(uint32_t*)e->data;
	if (expect("message length", length,  e->size)) {
		return;
	}

	Message *message = deSerializeMessage(e->data);
	PlayerConn *player = &match->players[playerIdx];
	switch (message->type) {
	case MSG_PLAY_REQUEST:
		if (expect("connected state", player->state, PLAYER_STATE_CONNECTED)) {
			return;
		}
		int otherPlayerIdx = 1 - playerIdx;

		break;
	case MSG_PLAY_CONFIRM:
		if (expect("ready state", player->state, PLAYER_STATE_PLAY_READY)) {
			return;
		}
	case MSG_PLAY_LEAVE:
		if (expect("playing state", player->state, PLAYER_STATE_PLAYING)) {
			return;
		}
		fprintf(stderr, "Leaving game unimplemented\n");
		break;
	case MSG_SEND_MOVE:
	}
}

void onError(dyad_Event *e) {
	fprintf(stderr, "ERROR: Server: %s\n", e->msg);
}
