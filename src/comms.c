#include "comms.h"

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dyad.h"

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
	} else if (message->type == MSG_START_GAME) {
		message->length += sizeof(message->player);
		memcpy(&message->player, data, sizeof(message->player));
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
		message->type != MSG_SEND_UPDATE &&
		message->type != MSG_START_GAME) {
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
	} else if (message->type == MSG_SEND_UPDATE) {
		memcpy(data, &message->update, sizeof(message->update));
	} else if (message->type == MSG_START_GAME) {
		memcpy(data, &message->player, sizeof(message->player));
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
			.source.x = *(uint8_t*)(data+0),
			.source.y = *(uint8_t*)(data+1),
			.target.x = *(uint8_t*)(data+2),
			.target.y = *(uint8_t*)(data+3),
		};
	} else if (message->type == MSG_SEND_UPDATE) {
		static_assert(sizeof(message->update) == 1);
		message->update = *(Update*)(data);
	} else if (message->type == MSG_START_GAME) {
		static_assert(sizeof(message->player) == 1);
		message->player = *(Player*)(data);
	} else if (message->length != offset) {
		message->data = strdup((char*)(data));
	}
	
	return message;
}

void sendMessage(void *stream_, MessageType type, const void *data)
{
	dyad_Stream *stream = stream_;
	Message *message = makeMessage(type, data);
	char buffer[128];
	serializeMessage(message, buffer);
	dyad_write(stream, buffer, message->length);
	freeMessage(message);
}

void initMessageQueue(MessageQueue *queue)
{
	queue->head = NULL;
    queue->tail = NULL;
    pthread_mutex_init(&queue->mutex, NULL);
}

void destroyMessageQueue(MessageQueue *queue)
{
    pthread_mutex_lock(&queue->mutex);

    MessageNode *current = queue->head;
    while (current != NULL) {
		freeMessage(current->message);
        MessageNode *temp = current;
        current = current->next;
        free(temp);
    }

    pthread_mutex_unlock(&queue->mutex);
    pthread_mutex_destroy(&queue->mutex);
}

void enqueueMessageQueue(MessageQueue *queue, Message *message)
{
	MessageNode *node = malloc(sizeof(MessageNode));
    if (!node) {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }
    node->message = message;
    node->next = NULL;

    pthread_mutex_lock(&queue->mutex);

    if (queue->tail) {
        queue->tail->next = node;
    } else {
        queue->head = node;
    }
    queue->tail = node;

    pthread_mutex_unlock(&queue->mutex);
}

Message *dequeueMessageQueue(MessageQueue *queue)
{
    pthread_mutex_lock(&queue->mutex);

    if (queue->head == NULL) {
        pthread_mutex_unlock(&queue->mutex);
        return NULL;
    }

    MessageNode *node = queue->head;
    Message *message = malloc(sizeof(Message));
    if (!message) {
        perror("Failed to allocate memory for message");
        pthread_mutex_unlock(&queue->mutex);
        exit(EXIT_FAILURE);
    }
    message = node->message;
    queue->head = node->next;

    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    free(node);
    pthread_mutex_unlock(&queue->mutex);

    return message;
}

void readMessagesToQueue(char *data, int size, MessageQueue *queue)
{
	int offset = 0;
	while (offset < size) {
		Message *message = deSerializeMessage(data+offset);
		enqueueMessageQueue(queue, message);

		// TODO: error checks
		offset += message->length;
	}
}

bool expect(const char *name, int expected, int value) {
	if (expected != value) {
		fprintf(stderr, "Expected %s(%d), got %d\n", name, expected, value);
		return true;
	}
	return false;
}
