#ifdef __cplusplus
extern "C" {
#endif

#ifndef UTILS_H
#define UTILS_H

#ifdef _WINDLL
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>

	DLL_API void clear_input_stream(FILE *);

#define _cout(p, x, ...) _ftprintf_s(p, _T(x), __VA_ARGS__)
#define cout(x, ...) _cout(stdout, x, __VA_ARGS__)
#define _cin(p, x, ...) {\
							_ftscanf_s(p, _T(x), __VA_ARGS__);\
							clear_input_stream(p);\
						}
#define cin(x, ...) _cin(stdin, x, __VA_ARGS__)
#define _cmp(str1, str2) _tcscmp(str1, str2)
#define cmp(str1, str2) _cmp(str1, _T(str2))
#define _icmp(str1, str2) _tcsicmp(str1, str2)
#define icmp(str1, str2) _tcsicmp(str1, _T(str2))
#define _contains(str, search) _tcsstr(str, search)
#define contains(str, search) _contains(str, _T(search))
#define _cpy(dest, src, size) _tcscpy_s(dest, size, src)
#define cpy(dest, src, size) _cpy(dest, _T(src), size)
#define itos(num, buffer, buffer_size) _itot_s(num, buffer, buffer_size, 10)
#define format(buffer, size, x, ...) _sntprintf_s(buffer, size, size, _T(x), __VA_ARGS__)

#define MAX_MAP 1000
#define MAX_NAME 50
#define MAX_SHARED_BUFFER 20

#define HEARTBEAT_TIMER 3000

#define MAX_AIRPORT 90
#define MAX_AIRPLANE 100
#define MAX_PASSENGER (100 * MAX_AIRPLANE)

#define DEFAULT_CIN_BUFFER "%49[^\n]"

#define MAX_TIMEOUT_SEND_COMMAND 10000

#define MTX_MEMORY _T("MTXSharedMemory")
#define FILE_MAPPING_NAME _T("ControlAviao")
#define SEM_EMPTY_C _T("SemaphoreEmptyC")
#define SEM_EMPTY_A _T("SemaphoreEmptyA")
#define SEM_ITEM_C _T("SemaphoreItemC")
#define SEM_ITEM_A _T("SemaphoreItemA")
#define STOP_SYSTEM_EVENT _T("StopEvent")

#define SEM_PIPE _T("SemaphorePipe")
#define PIPE_NAME _T("\\\\.\\pipe\\AeroSpace")

	// Command IDs
#define CMD_HELLO				0b0000000000000001
#define CMD_BOARD				0b0000000000000010
#define CMD_LIFT_OFF			0b0000000000000100
#define CMD_FLYING				0b0000000000001000
#define CMD_AVOID_COLLISION		0b0000000000010000
#define CMD_LANDED				0b0000000000100000
#define CMD_CRASHED_RETIRED		0b0000000001000000
#define CMD_SEND_DESTINATION	0b0000000010000000
#define CMD_OK					0b0000000100000000
#define CMD_HEARTBEAT			0b0000001000000000
#define CMD_ERROR				0b0000010000000000
#define CMD_SHUTDOWN			0b0000100000000000

	DLL_API typedef struct point {
		unsigned int x, y;
	} Point;

	DLL_API typedef struct airport {
		unsigned int id;					// 1 ~ 90
		unsigned int active : 1;
		TCHAR name[MAX_NAME];
		Point coordinates;					// 0 ~ 1000
	} Airport;

	DLL_API typedef struct airplane {
		unsigned int id;					// 91 ~ 190, MAYBE ADD PID?
		unsigned int pid;
		unsigned int active : 1;			// See if it is in use
		unsigned int alive : 1;				// For heartbeat
		TCHAR name[MAX_NAME];
		int velocity;
		int capacity;						// Current capacity
		int max_capacity;					// Maximum capacity
		Point coordinates;					// 0 ~ 1000
		Airport airport_start;
		Airport airport_end;
		BOOL boarding;
		BOOL flying;
	} Airplane;

	DLL_API typedef struct passenger {
		unsigned int id;					// 191 ~ 1190
		unsigned int active : 1;
		TCHAR name[MAX_NAME];
		DWORD wait_time;
		Airport airport;
		Airport airport_end;
		Airplane airplane;					// PID 0 not in airplane, != 0 in an airplane
		HANDLE pipe;
		HANDLE pipe_event;
		OVERLAPPED overlapped;
	} Passenger;

	DLL_API typedef union command {
		Point coordinates;
		TCHAR str[MAX_NAME];
		Airplane airplane;
		Airport airport;
		Passenger passenger;
		int number;
	} Command;

	DLL_API typedef struct sharedbuffer {
		unsigned int from_id;
		unsigned int to_id;
		unsigned int cmd_id;
		Command command;
	} SharedBuffer;

	DLL_API typedef struct sharedmemory {
		unsigned int max_airport;			// Maximum number of airports
		unsigned int map[MAX_MAP][MAX_MAP];	// IDs (0 = empty, 1 ~ 90 = airports, 91 ~ 190 = airplanes)
		BOOL accepting_planes;
		int inC, outC;
		SharedBuffer bufferControl[MAX_SHARED_BUFFER];
		int inA, outA;
		SharedBuffer bufferAirplane[MAX_SHARED_BUFFER];
	} SharedMemory;

	DLL_API typedef struct namedpipebuffer {
		unsigned int cmd_id;
		Command command;
	} NamedPipeBuffer;

	/*DLL_API BOOL createOrOpenRegistry(const TCHAR *subkey, HKEY *key, DWORD *result);
	DLL_API BOOL createOrEditRegistryValue(const TCHAR *subkey, const TCHAR *subvalue, DWORD dwType, HKEY *key);
	DLL_API BOOL viewRegistryValue(const TCHAR *subkey, const TCHAR *subvalue, DWORD *cbData, HKEY *key);
	DLL_API BOOL deleteRegistryValue(const TCHAR *subkey, HKEY *key);

	DLL_API BOOL createThread(HANDLE *h, LPTHREAD_START_ROUTINE routine, void *data, DWORD flags, DWORD *threadId);*/

#endif // !UTILS_H

#ifdef __cplusplus
}
#endif
