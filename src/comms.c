#include "comms.h"

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Message *makeMessage(MessageType type, const void *data)
{
	Message *message = malloc(sizeof(Message));
	message->length = sizeof(message->length) + sizeof(message->type);
	static_assert(sizeof(type) == 1);
	message->type = type;
	if (message->type == MSG_SEND_MOVE) {
		message->length += sizeof(message->move);
		memcpy(&message->move, data, sizeof(message->move));
	} else if (message->type == MSG_SEND_UPDATE) {
		message->length += sizeof(message->update);
		memcpy(&message->update, data, sizeof(message->update));
	} else {
		if (data != NULL) {
			message->data = strdup((char*)data);
			message->length += strlen(message->data) + 1;
		} else {
			message->data = NULL;
		}
	}
	return message;
}

void freeMessage(Message *message)
{
	if (message->type != MSG_SEND_MOVE &&
		message->type != MSG_SEND_UPDATE) {
	    free(message->data);
	}
	free(message);
}

void serializeMessage(const Message *message, void *data)
{
	static_assert(sizeof(message->length) == 4);
	*(uint32_t*)data = htonl(message->length);
	data += sizeof(message->length);

	static_assert(sizeof(message->type) == 1);
	*(uint8_t*)data = message->type;
	data += sizeof(message->type);

	if (message->type == MSG_SEND_MOVE) {
		memcpy(data, &message->move, sizeof(message->move));
	} if (message->type == MSG_SEND_UPDATE) {
		memcpy(data, &message->update, sizeof(message->update));
	} else if (message->data) {
	    strcpy(data, message->data);
	}
}

Message *deSerializeMessage(void *data)
{
	Message *message = malloc(sizeof(Message));

	static_assert(sizeof(message->length) == 4);
	message->length = ntohl(*(uint32_t*)data);
	data += sizeof(message->length);

	static_assert(sizeof(message->type) == 1);
	message->type = *(uint8_t*)data;
	data += sizeof(message->type);

    uint32_t offset = sizeof(message->length) + sizeof(message->type);
	if (message->type == MSG_SEND_MOVE) {
		static_assert(sizeof(message->move) == 4);
		message->move = (Move){
			.source.x = ntohl(*(uint8_t*)(data+0)),
			.source.y = ntohl(*(uint8_t*)(data+1)),
			.target.x = ntohl(*(uint8_t*)(data+2)),
			.target.y = ntohl(*(uint8_t*)(data+3)),
		};
	} else if (message->type == MSG_SEND_UPDATE) {
		static_assert(sizeof(message->update) == 1);
		message->update = *(Update*)(data);
	} else if (message->length != offset) {
		message->data = strdup((char*)(data));
	}
	
	return message;
}
