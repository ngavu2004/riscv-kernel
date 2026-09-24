#include <stdbool.h>
#include <stdint.h>

#define MESSAGE_SIZE 128
#define MESSAGE_COUNT 4

typedef struct {
	char message[MESSAGE_SIZE];
	int source_pid;
	int dest_pid;
} message_t;

typedef struct {
	message_t *queue[MESSAGE_COUNT];
	int message_count;
} message_queue_t;

extern message_t NULL_MESSAGE;

void initialize_message_queue(int p_num);
bool message_queue_is_full(message_queue_t q);
bool send(char *message, int source_pid, int dest_pid);
void receive(int pid, char *buffer);