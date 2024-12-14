#include "comms.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Message *makeMessage(MessageType type, const void *data)
{
	Message *message = malloc(sizeof(Message));
	message->length = sizeof(message->length) + sizeof(message->type);
	message->type = type;
	if (message->type == MSG_SEND_MOVE ||
		message->type == MSG_SEND_MOVE_GAME_END) {
		message->length += sizeof(message->move);
		memcpy(&message->move, data, sizeof(message->move));
	} else  {
		message->data = strdup((char*)data);
		message->length += strlen(message->data) + 1;
	}
	return message;
}

void freeMessage(Message *message)
{
	if (message->type != MSG_SEND_MOVE &&
		message->type != MSG_SEND_MOVE_GAME_END) {
	    free(message->data);
	}
	free(message);
}

void serializeMessage(const Message *message, void *data)
{
	*(uint32_t*)data = htonl(message->length);
	data += sizeof(message->data);
	if (sizeof(message->type) == 1) {
		*(uint8_t*)data = message->type;
		data += sizeof(message->type);
	} else {
		fprintf(stderr, "WELP: %zu\n", sizeof(message->type));
		exit(1);
	}
	if (message->type == MSG_SEND_MOVE ||
		message->type == MSG_SEND_MOVE_GAME_END) {
		memcpy(data, message, sizeof(*message));
	} else {
	    strcpy(data, message->data);
		((char*)data)[message->length-1] = 0;
	}
}

Message *deSerializeMessage(void *data)
{
	Message *message = malloc(sizeof(Message));
	// read first two fields
	int offset = sizeof(message->length) + sizeof(message->type);
	memcpy(message, data, offset);
	message->length = ntohl(message->length);
	if (message->type == MSG_SEND_MOVE ||
		message->type == MSG_SEND_MOVE_GAME_END) {
		message->move = (Move){
			.source.x = ntohl(*(uint8_t*)(data+offset+0)),
			.source.y = ntohl(*(uint8_t*)(data+offset+1)),
			.target.x = ntohl(*(uint8_t*)(data+offset+2)),
			.target.y = ntohl(*(uint8_t*)(data+offset+3)),
		};
	} else {
		message->data = strdup((char*)(data+offset));
	}
	
	return message;
}
