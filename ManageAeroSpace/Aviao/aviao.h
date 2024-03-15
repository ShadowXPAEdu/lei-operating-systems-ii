#ifndef AVIAO_H
#define AVIAO_H

#include "../Utils/utils.h"

#define DLL_AVIAO _T("SO2_TP_DLL_2021.dll")
#define DLL_FUNC_MOVE "move"

#define MTX_C _T("AviaoMutexC")
#define MTX_A _T("AviaoMutexA")

typedef struct cfg {
	BOOL die;
	int (*move) (int, int, int, int, int *, int *);
	Airplane airplane;
	SharedMemory *memory;				// MapViewOfFile
	HMODULE hdll;
	// Handles
	HANDLE obj_map;
	HANDLE mtx_memory;					// Mutex for shared memory access
	HANDLE stop_event;					// Kill event (Stops the whole system)
	HANDLE stop_airplane;				// Kill airplane
	/*
		Semaphores and circular buffer handlers
	*/
	HANDLE sem_emptyC;
	HANDLE sem_itemC;
	HANDLE mtx_C;
	HANDLE sem_emptyA;
	HANDLE sem_itemA;
	HANDLE mtx_A;
} Config;

int init_config(Config *);
void end_config(Config *);
void init_aviao(Config *);

BOOL receive_command_sharedmemory(Config *, SharedBuffer *);
BOOL send_command_sharedmemory(Config *, SharedBuffer *);

DWORD WINAPI read_command(void *);
DWORD WINAPI read_shared_memory(void *);
DWORD WINAPI send_heartbeat(void *);
DWORD WINAPI flying(void *);

#endif // !AVIAO_H
