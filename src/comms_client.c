#include "comms_client.h"

#include <arpa/inet.h>
#include <stddef.h>
#include <stdio.h>

#include "board.h"
#include "comms.h"
#include "multiscreen.h"
#include "playscreen.h"

dyad_Stream *clientStream = NULL;
MessageQueue msgQueue;
PlayerState playerState = PLAYER_STATE_NONE;

void onConnect(dyad_Event *e) {
	dyad_setNoDelay(e->stream, true);
	playerState = PLAYER_STATE_CONNECTED;
}

void onData(dyad_Event *e)
{
	readMessagesToQueue(e->data, e->size, &msgQueue);
}

void onError(dyad_Event *e) {
	fprintf(stderr, "ERROR: Client: %s\n", e->msg);
}
