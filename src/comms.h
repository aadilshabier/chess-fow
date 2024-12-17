#ifndef COMMS_H_
#define COMMS_H_

#include <stdint.h>

#include "piece.h"

enum MessageType {
	MSG_REQUEST_GAME,
	MSG_REQUEST_RECVD,
	MSG_START_GAME,
	MSG_SEND_MOVE,
	MSG_SEND_UPDATE,
	MSG_ERROR,
	MSG_LEAVE_GAME,
} __attribute__ ((packed));
typedef enum MessageType MessageType;

enum PlayerState {
	PLAYER_STATE_NONE,
	PLAYER_STATE_CONNECTED,
	PLAYER_STATE_SENT_REQ,
	PLAYER_STATE_READY,
	PLAYER_STATE_PLAYING,
	PLAYER_STATE_ENDED,
}  __attribute__ ((packed));
typedef enum PlayerState PlayerState;

// TODO: can replace i32 with i8
typedef struct Move {
	struct {
		uint8_t x, y;
	} source, target;
} Move;

enum Update {
	UPDATE_CHECK,
	UPDATE_CHECKMATE,
} __attribute__ ((packed));
typedef enum Update Update;

typedef struct Message {
	uint32_t length;
	MessageType type;
	/* uint64_t key; */
    union {
		Player player;
		char *data;
		Move move;
		Update update;
	};
} Message;

Message *makeMessage(MessageType type, const void *data);
void freeMessage(Message *message);
void serializeMessage(const Message *message, void *data);
Message sendMessage(void *stream, MessageType type, const void *data);
Message *deSerializeMessage(void *data);

#endif // COMMS_H_
