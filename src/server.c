#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "comms_server.h"
#include "dyad.h"

const char *HOST = "0.0.0.0";
const int PORT = 12345;

Match match = {0};

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
	}

	dyad_shutdown();
}
