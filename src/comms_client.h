#ifndef COMMS_CLIENT_H_
#define COMMS_CLIENT_H_

#include "comms.h"
#include "dyad.h"

typedef enum MultiPlayerMode {
	MULTI_MODE_WHITE,
	MULTI_MODE_RANDOM,
	MULTI_MODE_BLACK,
} MultiPlayerMode;

void onData(dyad_Event *e);
void onConnect(dyad_Event *e);
void onError(dyad_Event *e);

extern dyad_Stream *clientStream;
extern MessageQueue msgQueue;
extern PlayerState playerState;

#endif // COMMS_CLIENT_H_
