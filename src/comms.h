#ifndef COMMS_H_
#define COMMS_H_

#include <stdint.h>

#include "piece.h"

enum MessageType {
	MSG_PLAY_REQUEST,
	MSG_PLAY_RESPONSE,
	MSG_PLAY_CONFIRM,
	MSG_PLAY_START,
	MSG_PLAY_ERROR,
	MSG_PLAY_LEAVE,
	MSG_SEND_MOVE,
	// TODO: this could be much better
	MSG_SEND_MOVE_GAME_END,
} __attribute__ ((packed));
typedef enum MessageType MessageType;

// TODO: can replace i32 with i8
typedef struct Move {
	struct {
		uint8_t x, y;
	} source, target;
} Move;

typedef struct Message {
	uint32_t length;
	MessageType type;
	/* uint64_t key; */
    union {
		Move move;
		char *data;
	};
} Message;

Message *makeMessage(MessageType type, const void *data);
void freeMessage(Message *message);
void serializeMessage(const Message *message, void *data);
Message *deSerializeMessage(void *data);

#endif // COMMS_H_
