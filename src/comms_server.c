#include "comms_server.h"

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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

void onListen(dyad_Event *e) {
	printf("Server listening on localhost:%d\n", dyad_getPort(e->stream));
}

void onAccept(dyad_Event *e) {
	Match *match = e->udata;
	if (match->activePlayers >= 2) {
		const char *content = "ERROR: No space in the server!";
		fprintf(stderr, "%s\n", content);
		Message *message = makeMessage(MSG_ERROR, content);
		char buffer[1024];
		serializeMessage(message, buffer);
		dyad_write(e->remote, &message, sizeof(message));
		freeMessage(message);
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
	int playerIdx = -1;
	if (match->players[0].stream == stream) playerIdx = 0;
	else if (match->players[1].stream == stream) playerIdx = 1;
	else {
		fprintf(stderr, "ERROR: fd = %d is nout found\n", dyad_getSocket(stream));
		return;
	}

	uint32_t length = ntohl(*(uint32_t*)e->data);
	if (expect("message length", length,  e->size)) {
		return;
	}

	Message *message = deSerializeMessage(e->data);
	PlayerConn *player = &match->players[playerIdx];
	MessageType type = message->type;
	if (type == MSG_REQUEST_GAME) {
		if (expect("connected state", player->state, PLAYER_STATE_CONNECTED)) {
			return;
		}
		// TODO: read preference
		Message *message = makeMessage(MSG_REQUEST_RECVD, NULL);
		char buffer[1024];
		serializeMessage(message, buffer);
		dyad_write(player->stream, buffer, message->length);
		freeMessage(message);

		player->state = PLAYER_STATE_SENT_REQ;
	} else if (type == MSG_SEND_MOVE) {
		if (expect("ready state", player->state, PLAYER_STATE_PLAYING)) {
			return;
		}
	} else if (type == MSG_SEND_UPDATE) {
		if (expect("ready state", player->state, PLAYER_STATE_PLAYING)) {
			return;
		}
	} else if (type == MSG_LEAVE_GAME) {
		if (expect("playing state", player->state, PLAYER_STATE_PLAYING)) {
			return;
		}
		fprintf(stderr, "Leaving game unimplemented\n");
	} else if (type == MSG_ERROR) {
		const char *data = message->data;
		fprintf(stderr, "ERROR: %s\n", data);
	} else {
		fprintf(stderr, "Shouldn't happen: %d\n", type);
	}
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

