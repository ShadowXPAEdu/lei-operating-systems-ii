#include "passag.h"

BOOL init_config(Config *cfg) {
	memset(cfg, 0, sizeof(Config));

	cfg->passenger.pipe_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (cfg->passenger.pipe_event == NULL)
		return FALSE;

	cfg->passenger.overlapped.hEvent = cfg->passenger.pipe_event;

	cfg->sem_pipe = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEM_PIPE);
	if (cfg->sem_pipe == NULL)
		return FALSE;

	cfg->board_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (cfg->board_event == NULL)
		return FALSE;

	cfg->stop_event = OpenEvent(EVENT_ALL_ACCESS, FALSE, STOP_SYSTEM_EVENT);
	if (cfg->stop_event == NULL)
		return FALSE;

	cfg->stop_passag = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (cfg->stop_passag == NULL)
		return FALSE;

	return TRUE;
}

void end_config(Config *cfg) {
	CloseHandle(cfg->sem_pipe);
	FlushFileBuffers(cfg->passenger.pipe);
	DisconnectNamedPipe(cfg->passenger.pipe);
	CloseHandle(cfg->passenger.pipe);
	CloseHandle(cfg->passenger.pipe_event);
	CloseHandle(cfg->stop_event);
	CloseHandle(cfg->stop_passag);
}

void init_passag(Config *cfg) {
	// init named pipe
	int tries = 0;
	do {
		cfg->passenger.pipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

		if (cfg->passenger.pipe != INVALID_HANDLE_VALUE) {
			break;
		}

		if (GetLastError() != ERROR_PIPE_BUSY) {
			cout("Not possible to connect to named pipe! Busy!\n");
			tries++;
		}

		if (!WaitNamedPipe(PIPE_NAME, PIPE_TIMEOUT)) {
			cout("Not possible to connect to named pipe! Timeout.\n");
			tries++;
		}

		if (tries > 5) {
			cout("Tries enough times... Exiting...\n");
			return;
		}
	} while (TRUE);
	DWORD dwMode = PIPE_READMODE_MESSAGE;
	if (!SetNamedPipeHandleState(cfg->passenger.pipe, &dwMode, NULL, NULL)) {
		cout("Not able to set named pipe handle state!\n");
		return;
	}
	ReleaseSemaphore(cfg->sem_pipe, 1, NULL);

	// send hello!
	NamedPipeBuffer buff;
	buff.cmd_id = CMD_HELLO;
	buff.command.passenger = cfg->passenger;
	if (!send_message_namedpipe(cfg, &buff)) {
		cout("Error sending message to control!\n");
		return;
	}
	if (!receive_message_namedpipe(cfg, &buff)) {
		cout("Error receiving message from control!\n");
		return;
	}
	if (buff.cmd_id == (CMD_HELLO | CMD_OK)) {
		Passenger p = buff.command.passenger;
		cfg->passenger.id = p.id;
		cfg->passenger.active = p.active;
		cfg->passenger.airplane = p.airplane;
		cfg->passenger.airport = p.airport;
		cfg->passenger.airport_end = p.airport_end;
		cout("Connected!\n");
		cout("Departure: '%s' (x: %u, y: %u)\nLanding: '%s' (x: %u, y: %u)\n",
			cfg->passenger.airport.name, cfg->passenger.airport.coordinates.x, cfg->passenger.airport.coordinates.y,
			cfg->passenger.airport_end.name, cfg->passenger.airport_end.coordinates.x, cfg->passenger.airport_end.coordinates.y);
	} else if (buff.cmd_id == (CMD_HELLO | CMD_ERROR)) {
		cout("Error from control: '%s'\n", buff.command.str);
		return;
	} else {
		cout("Error connecting to control! Error code: %u\n", buff.cmd_id);
		return;
	}
	// init threads
	HANDLE threadCmd = CreateThread(NULL, 0, read_command, cfg, 0, NULL);
	if (threadCmd == NULL)
		return;
	HANDLE thread = CreateThread(NULL, 0, read_named_pipes, cfg, 0, NULL);
	if (thread == NULL)
		return;
	HANDLE threadTimer = NULL;
	if (cfg->passenger.wait_time != INFINITE) {
		threadTimer = CreateThread(NULL, 0, wait_time, cfg, 0, NULL);
		if (threadTimer == NULL)
			return;
	}

	WaitForSingleObject(thread, INFINITE);

	CloseHandle(threadCmd);
	CloseHandle(thread);
	if (threadTimer != NULL)
		CloseHandle(threadTimer);
}

DWORD WINAPI wait_time(void *param) {
	Config *cfg = (Config *) param;
	HANDLE handles[3];
	handles[0] = cfg->stop_event;
	handles[1] = cfg->stop_passag;
	handles[2] = cfg->board_event;

	DWORD res = WaitForMultipleObjects(3, handles, FALSE, cfg->passenger.wait_time * 1000);

	if (res == WAIT_TIMEOUT) {
		NamedPipeBuffer npBuffer;
		npBuffer.cmd_id = CMD_CRASHED_RETIRED;
		send_message_namedpipe(cfg, &npBuffer);
		SetEvent(cfg->stop_passag);
	}

	return 0;
}

DWORD WINAPI read_command(void *param) {
	Config *cfg = (Config *) param;
	TCHAR buffer[MAX_NAME] = { 0 };
	do {
		cout("Input command:\n > ");
		cin(DEFAULT_CIN_BUFFER, buffer, MAX_NAME);

		if (icmp(buffer, "exit") == 0) {
			cfg->die = TRUE;
			NamedPipeBuffer npBuffer;
			npBuffer.cmd_id = CMD_CRASHED_RETIRED;
			send_message_namedpipe(cfg, &npBuffer);
		} else {
			cout("Invalid command!\n");
		}
	} while (!cfg->die);
	SetEvent(cfg->stop_passag);
	return 0;
}

DWORD WINAPI read_named_pipes(void *param) {
	Config *cfg = (Config *) param;
	NamedPipeBuffer npBuffer;
	while (!cfg->die) {
		if (receive_message_namedpipe(cfg, &npBuffer)) {
			switch (npBuffer.cmd_id) {
				case (CMD_BOARD):
				{
					SetEvent(cfg->board_event);
					cfg->passenger.airplane = npBuffer.command.airplane;
					cout("Passenger '%s' has boarded into airplane: '%s'\nDeparture: '%s' (x: %u, y: %u)\nDestination: '%s' (x: %u, y: %u)\n",
						cfg->passenger.name, cfg->passenger.airplane.name, cfg->passenger.airport.name, cfg->passenger.airport.coordinates.x, cfg->passenger.airport.coordinates.y,
						cfg->passenger.airport_end.name, cfg->passenger.airport_end.coordinates.x, cfg->passenger.airport_end.coordinates.y);
					break;
				}
				case (CMD_FLYING):
				{
					cfg->passenger.airplane = npBuffer.command.airplane;
					cout("Passenger '%s' is flying on airplane '%s' (x: %u, y: %u)\n", cfg->passenger.name, cfg->passenger.airplane.name, cfg->passenger.airplane.coordinates.x, cfg->passenger.airplane.coordinates.y);
					break;
				}
				case (CMD_LANDED):
				{
					cout("Passenger '%s' arrived safely at its destination!\nAirport: '%s' (x: %u, y: %u)", cfg->passenger.name, cfg->passenger.airport_end.name, cfg->passenger.airport_end.coordinates.x, cfg->passenger.airport_end.coordinates.y);
					cfg->die = TRUE;
					break;
				}
				case (CMD_CRASHED_RETIRED):
				{
					cfg->passenger.airplane = npBuffer.command.airplane;
					cout("Passenger '%s' has died in a horrible accident!\nAirplane '%s' crashed at: (x: %u, y: %u)", cfg->passenger.name, cfg->passenger.airplane.name, cfg->passenger.airplane.coordinates.x, cfg->passenger.airplane.coordinates.y);
					cfg->die = TRUE;
					break;
				}
				case (CMD_SHUTDOWN):
				{
					cout("Forcefully shut down by control!\n");
					cfg->die = TRUE;
					break;
				}
				default:
				{
					break;
				}
			}
		} else {
			cfg->die = TRUE;
			cout("System has been stopped!\n");
		}
	}

	return 0;
}

BOOL receive_message_namedpipe(Config *cfg, NamedPipeBuffer *npBuffer) {
	DWORD read;
	HANDLE handles[3];
	handles[0] = cfg->stop_event;
	handles[1] = cfg->stop_passag;
	handles[2] = cfg->passenger.pipe_event;
	BOOL ignore = ReadFile(cfg->passenger.pipe, npBuffer, sizeof(NamedPipeBuffer), &read, &cfg->passenger.overlapped);
	if (ignore) {}
	DWORD res = WaitForMultipleObjects(3, handles, FALSE, INFINITE);
	if (res == WAIT_OBJECT_0 || res == (WAIT_OBJECT_0 + 1)) {
		// Ignore
		cout("System is down!\n");
		return FALSE;
	} else {
		GetOverlappedResult(cfg->passenger.pipe, &cfg->passenger.overlapped, &read, FALSE);
		if (read != sizeof(NamedPipeBuffer)) {
			cout("Amount read is different from expected!\n");
			return FALSE;
		}
	}
	return TRUE;
}

BOOL send_message_namedpipe(Config *cfg, NamedPipeBuffer *npBuffer) {
	DWORD written;
	HANDLE handles[3];
	handles[0] = cfg->stop_event;
	handles[1] = cfg->stop_passag;
	handles[2] = cfg->passenger.pipe_event;
	BOOL success = WriteFile(cfg->passenger.pipe, npBuffer, sizeof(NamedPipeBuffer), &written, &cfg->passenger.overlapped);
	DWORD res = WaitForMultipleObjects(3, handles, FALSE, MAX_TIMEOUT_SEND_COMMAND);
	if (res == WAIT_OBJECT_0 || res == (WAIT_OBJECT_0 + 1)) {
		// Ignore
		return FALSE;
	} else if (res == WAIT_TIMEOUT) {
		cout("Can not connect to control or named pipe.\nExiting...\n");
		cfg->die = TRUE;
		SetEvent(cfg->stop_passag);
		return FALSE;
	} else {
		GetOverlappedResult(cfg->passenger.pipe, &cfg->passenger.overlapped, &written, FALSE);
		if (written != sizeof(NamedPipeBuffer)) {
			cout("Amount written is different from expected!\n");
			return FALSE;
		}
	}
	return TRUE;
}
