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
	return conn;
}

void onListen(dyad_Event *e) {
	printf("Server listening on localhost:%d\n", dyad_getPort(e->stream));
}

static bool readyToPlay(Match *match)
{
	if (match->activePlayers < 2)
		return false;

	for (int i=0; i<2; i++) {
		if (match->players[i].state != PLAYER_STATE_SENT_REQ)
			return false;
	}

	return true;
}

static void broadcastStartGame(Match *match)
{
	for (int i=0; i<2; i++) {
		PlayerConn *player = &match->players[i];
		player->state = PLAYER_STATE_PLAYING;
		// TODO: Randomize
		player->player = (Player)(i+1);
		sendMessage(player->stream, MSG_START_GAME, &player->player);
	}
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

static bool expect(const char *name, int expected, int value) {
	if (expected != value) {
		fprintf(stderr, "Expected %s(%d), got %d\n", name, expected, value);
		return true;
	}
	return false;
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

	uint32_t length = ntohl(*(uint32_t*)e->data);
	if (expect("message length", length,  e->size)) {
		return;
	}

	Message *message = deSerializeMessage(e->data);
	PlayerConn *player = &match->players[playerIdx];
	MessageType type = message->type;
	if (type == MSG_REQUEST_GAME) {
		if (expect("connected state", PLAYER_STATE_CONNECTED, player->state)) {
			return;
		}
		// TODO: read preference
		sendMessage(player->stream, MSG_REQUEST_RECVD, NULL);

		player->state = PLAYER_STATE_SENT_REQ;

		// hack to hide the fact that I don't have a stream reader :)
		// TODO: use a message queue instead
		dyad_update();
		// required to stop the messages from coalescing
		sleep(1);
		if (readyToPlay(match)) {
			broadcastStartGame(match);
		}
	} else if (type == MSG_SEND_MOVE) {
		if (expect("ready state", PLAYER_STATE_PLAYING, player->state)) {
			return;
		}
		int otherPlayerIdx = 1 - playerIdx;
		dyad_write(match->players[otherPlayerIdx].stream, e->data, message->length);
	} else if (type == MSG_SEND_UPDATE) {
		if (expect("ready state", PLAYER_STATE_PLAYING, player->state)) {
			return;
		}
		int otherPlayerIdx = 1 - playerIdx;
		dyad_write(match->players[otherPlayerIdx].stream, e->data, message->length);
	} else if (type == MSG_LEAVE_GAME) {
		if (expect("playing state", PLAYER_STATE_PLAYING, player->state)) {
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

