#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "comms_server.h"
#include "dyad.h"

const char *HOST = "0.0.0.0";
const int PORT = 12345;

ServerGamesManager gamesManager = {0};

static bool readyToPlay(Game *game)
{
	if (game->numPlayers < 2)
		return false;

	for (int i=0; i<2; i++) {
		if (game->players[i].state != PLAYER_STATE_SENT_REQ)
			return false;
	}

	return true;
}

static void broadcastStartGame(Game *game)
{
	for (int i=0; i<2; i++) {
		PlayerConn *player = &game->players[i];
		player->state = PLAYER_STATE_PLAYING;
		// TODO: Randomize
		player->player = (Player)(i+1);
		sendMessage(player->stream, MSG_START_GAME, &player->player);
	}
}

// TODO: Maybe rename and move
static void handleGameEvents(Game *game) {
	if (game->numPlayers != 2)
		return;

	for (int i=0; i<2; i++) {
		MessageQueue *queue = &game->players[i].queue;
		PlayerConn *player = &game->players[i];
		// handle all events
		while (true) {
			Message *message = dequeueMessageQueue(queue);
			if (!message) break;

			MessageType type = message->type;
			if (type == MSG_REQUEST_GAME) {
				if (expect("connected state", PLAYER_STATE_CONNECTED, player->state)) {	
					return /*1*/;
				}
				// TODO: read preference
				sendMessage(player->stream, MSG_REQUEST_RECVD, NULL);

				player->state = PLAYER_STATE_SENT_REQ;

				if (readyToPlay(game)) {
					broadcastStartGame(game);
				}
			} else if (type == MSG_SEND_MOVE) {
				if (expect("ready state", PLAYER_STATE_PLAYING, player->state)) {
					return /*1*/;
				}
				int otherPlayerIdx = 1 - i;
				sendMessage(game->players[otherPlayerIdx].stream, type, &message->move);
			} else if (type == MSG_SEND_UPDATE) {
				if (expect("ready state", PLAYER_STATE_PLAYING, player->state)) {
					return /*1*/;
				}
				int otherPlayerIdx = 1 - i;
				sendMessage(game->players[otherPlayerIdx].stream, type, &message->update);
			} else if (type == MSG_LEAVE_GAME) {
				if (expect("playing state", PLAYER_STATE_PLAYING, player->state)) {
					return /*1*/;
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

int main()
{
	dyad_init();
	srandom((unsigned)time(NULL));
	dyad_Stream *server = dyad_newStream();
	dyad_addListener(server, DYAD_EVENT_ERROR,  onError,  NULL);
	dyad_addListener(server, DYAD_EVENT_LISTEN,  onListen,  NULL);
	dyad_addListener(server, DYAD_EVENT_ACCEPT, onAccept, &gamesManager);
	dyad_addListener(server, DYAD_EVENT_CLOSE, onServerClose, &gamesManager);
	if (dyad_listenEx(server, HOST, PORT, 10) < 0) {
		return 1;
	}

	while (dyad_getStreamCount() > 0) {
		dyad_update();

		// change this loop to instead:
		// 1. look through all active games
		// 2. read their queues
		// 3. perform the game logic below (move elsewhere)

		// TODO: run these in seperate threads which wake up message queue is not empty
		for (int i = 0; i < gamesManager.numGames; i++) {
			Game *game = &gamesManager.games[i];
			handleGameEvents(game);
		}
	}

	dyad_shutdown();
}
