#include "multiscreen.h"

#include <raylib.h>

#include <stddef.h>
#include <stdlib.h>

#include "comms.h"
#include "comms_client.h"
#include "dyad.h"
#include "loadingscreen.h"

GameState _MULTISTATEOBJ = {
	.init = &InitMultiScreen,
	.update = &UpdateMultiScreen,
	.draw = &DrawMultiScreen,
	.exit = NULL,
};

extern const int screenWidth;
extern const int screenHeight;
extern const Color bgColor;

static const int fontSize = 40;
static PlayerToggleState playerToggle;

const char* HOST = "127.0.0.1";
const int PORT = 12345;
Player clientPlayer;

void InitMultiScreen(void *playerTogglePtr)
{
	playerToggle = *(PlayerToggleState*)playerTogglePtr;
	playerState = PLAYER_STATE_NONE;
	clientStream = NULL;
}


static void DrawTextMiddle(const char *text)
{
	int width = MeasureText(text, fontSize);
	DrawText(text, (screenWidth-width)/2, screenHeight/2, fontSize, BLACK);
}

#include <stdio.h>

void UpdateMultiScreen()
{
	dyad_update();

	if (playerState == PLAYER_STATE_NONE) {
		clientStream = dyad_newStream();
		dyad_setNoDelay(clientStream, true);
		dyad_setUpdateTimeout(1.f/30.f);
		/* dyad_setTimeout(clientStream, 1); */
		dyad_addListener(clientStream, DYAD_EVENT_ERROR,  onError,  NULL);
		dyad_addListener(clientStream, DYAD_EVENT_CONNECT, onConnect, NULL);
		dyad_addListener(clientStream, DYAD_EVENT_DATA, onData, NULL);

		if (dyad_connect(clientStream, HOST, PORT) < 0) {
			// error
			TraceLog(LOG_ERROR, "Connect failed to %s:%d", HOST, PORT);
		}
	} else if (playerState == PLAYER_STATE_CONNECTED) {
		// TODO: send preference
	    sendMessage(clientStream, MSG_REQUEST_GAME, NULL);
		playerState = PLAYER_STATE_SENT_REQ;
	} else if (playerState == PLAYER_STATE_PLAYING) {
		replaceGameState(GAME_STATE_PLAY, &clientPlayer);
	}
}

void DrawMultiScreen()
{
	ClearBackground(bgColor);
	if (playerState == PLAYER_STATE_NONE) {
	    DrawTextMiddle("Connecting...");
	} else if (playerState == PLAYER_STATE_CONNECTED) {
	    DrawTextMiddle("Connected...");
	} else if (playerState == PLAYER_STATE_SENT_REQ) {
	    DrawTextMiddle("Requested Game...");
	} else if (playerState == PLAYER_STATE_READY) {
	    DrawTextMiddle("Waiting to start...");
	}
}
