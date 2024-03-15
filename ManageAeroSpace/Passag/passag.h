#ifndef PASSAG_H
#define PASSAG_H

#include "../Utils/utils.h"

#define PIPE_TIMEOUT 10000

typedef struct cfg {
	BOOL die;
	Passenger passenger;
	HANDLE board_event;
	HANDLE stop_event;
	HANDLE stop_passag;
	HANDLE sem_pipe;
} Config;

BOOL init_config(Config *);
void end_config(Config *);
void init_passag(Config *);

BOOL receive_message_namedpipe(Config *, NamedPipeBuffer *);
BOOL send_message_namedpipe(Config *, NamedPipeBuffer *);

DWORD WINAPI read_command(void *);
DWORD WINAPI read_named_pipes(void *);
DWORD WINAPI wait_time(void *);

#endif // !PASSAG_H
