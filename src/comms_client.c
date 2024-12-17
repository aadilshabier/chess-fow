#include "comms_client.h"

#include <arpa/inet.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "comms.h"
#include "multiscreen.h"

dyad_Stream *clientStream = NULL;
PlayerState playerState = PLAYER_STATE_NONE;

void onConnect(dyad_Event *e) {
	dyad_setNoDelay(e->stream, true);
	playerState = PLAYER_STATE_CONNECTED;
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
	uint32_t length = ntohl(*(uint32_t*)e->data);
	if (expect("message length", length,  e->size)) {
		return;
	}

	Message *message = deSerializeMessage(e->data);
	/* PlayerConn *player = &match->players[playerIdx]; */
	MessageType type = message->type;
	if (type == MSG_REQUEST_RECVD) {
		if (expect("connected state", playerState, PLAYER_STATE_SENT_REQ)) {
			return;
		}
		playerState = PLAYER_STATE_READY;
	} else if (type == MSG_START_GAME) {
		if (expect("ready state", playerState, PLAYER_STATE_SENT_REQ)) {
			return;
		}
		playerState = PLAYER_STATE_PLAYING;
	} else if (type == MSG_SEND_MOVE) {
		fprintf(stderr, "Move unimplemented\n");
	} else if (type == MSG_SEND_UPDATE) {
		fprintf(stderr, "Update unimplemented\n");
	} else if (type == MSG_LEAVE_GAME) {
		if (expect("playing state", playerState, PLAYER_STATE_PLAYING)) {
			return;
		}
		fprintf(stderr, "Leaving game unimplemented\n");
	} else if (type == MSG_ERROR){
		fprintf(stderr, "ERROR: %s\n", message->data);
	} else {
		fprintf(stderr, "Shouldn't happen: %d\n", type);
	}
}

void onError(dyad_Event *e) {
	fprintf(stderr, "ERROR: Client: %s\n", e->msg);
}
