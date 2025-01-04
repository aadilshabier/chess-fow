#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "comms_server.h"
#include "dyad.h"

const char *HOST = "0.0.0.0";
const int PORT = 12345;

Match match = {0};

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

int main()
{
	dyad_init();
	srandom((unsigned)time(NULL));
	dyad_Stream *server = dyad_newStream();
	dyad_addListener(server, DYAD_EVENT_ERROR,  onError,  NULL);
	dyad_addListener(server, DYAD_EVENT_LISTEN,  onListen,  NULL);
	dyad_addListener(server, DYAD_EVENT_ACCEPT, onAccept, &match);
	dyad_addListener(server, DYAD_EVENT_CLOSE, onClose, &match);
	if (dyad_listenEx(server, HOST, PORT, 10) < 0) {
		return 1;
	}

	while (dyad_getStreamCount() > 0) {
		dyad_update();

		// TODO: run these in seperate threadw which wake up message queue is not empty
		if (match.activePlayers != 2)
			continue;

		for (int i=0; i<2; i++) {
			MessageQueue *queue = &match.players[i].queue;
			PlayerConn *player = &match.players[i];
			while (true) {
				Message *message = dequeueMessageQueue(queue);
				if (!message) break;

				MessageType type = message->type;
				if (type == MSG_REQUEST_GAME) {
					if (expect("connected state", PLAYER_STATE_CONNECTED, player->state)) {
						return 1;
					}
					// TODO: read preference
					sendMessage(player->stream, MSG_REQUEST_RECVD, NULL);

					player->state = PLAYER_STATE_SENT_REQ;

					if (readyToPlay(&match)) {
						broadcastStartGame(&match);
					}
				} else if (type == MSG_SEND_MOVE) {
					if (expect("ready state", PLAYER_STATE_PLAYING, player->state)) {
						return 1;
					}
					int otherPlayerIdx = 1 - i;
					sendMessage(match.players[otherPlayerIdx].stream, type, &message->move);
				} else if (type == MSG_SEND_UPDATE) {
					if (expect("ready state", PLAYER_STATE_PLAYING, player->state)) {
						return 1;
					}
					int otherPlayerIdx = 1 - i;
					sendMessage(match.players[otherPlayerIdx].stream, type, &message->update);
				} else if (type == MSG_LEAVE_GAME) {
					if (expect("playing state", PLAYER_STATE_PLAYING, player->state)) {
						return 1;
					}
					fprintf(stderr, "Leaving game unimplemented\n");
				} else if (type == MSG_ERROR) {
					const char *data = message->data;
					fprintf(stderr, "ERROR: %s\n", data);
				} else {
					fprintf(stderr, "Shouldn't happen: %d\n", type);
				}
			}
		}
	}

	dyad_shutdown();
}
