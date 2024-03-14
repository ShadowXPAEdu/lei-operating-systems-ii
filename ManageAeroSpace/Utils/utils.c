#include "utils.h"

void clear_input_stream(FILE * p) {
	int ch;
	while ((ch = _gettc(p)) != '\n' && ch != EOF);
}

//BOOL createOrOpenRegistry(const TCHAR *subkey, HKEY *key, DWORD *result) {
//	LSTATUS res = RegCreateKeyEx(
//		HKEY_CURRENT_USER,			//HKEY                        hKey,
//		subkey,						//LPCSTR                      lpSubKey,
//		0,							//DWORD                       Reserved,
//		NULL,						//LPSTR                       lpClass,
//		REG_OPTION_NON_VOLATILE,	//DWORD                       dwOptions,
//		KEY_ALL_ACCESS,				//REGSAM                      samDesired,
//		NULL,						//const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
//		key,						//PHKEY                       phkResult,
//		result						//LPDWORD                     lpdwDisposition
//	);
//
//	return (res == ERROR_SUCCESS);
//}
//
//BOOL createOrEditRegistryValue(const TCHAR *subkey, const TCHAR *subvalue, DWORD dwType, HKEY *key) {
//	LSTATUS res = RegSetValueEx(
//		*key,									//HKEY hKey,
//		subkey,									//LPCWSTR lpValueName,
//		0,										//DWORD Reserved,
//		dwType,									//DWORD dwType,
//		(const BYTE *) subvalue,				//const BYTE * lpData,
//		sizeof(TCHAR) * (_tcslen(subvalue) + 1)	//DWORD cbData);
//	);
//
//	return (res == ERROR_SUCCESS);
//}
//
//BOOL viewRegistryValue(const TCHAR *subkey, const TCHAR *subvalue, DWORD *cbData, HKEY *key) {
//	LSTATUS res = RegQueryValueEx(
//		*key,				//HKEY    hKey,
//		subkey,				//LPCSTR  lpValueName,
//		0,					//LPDWORD lpReserved,
//		NULL,				//LPDWORD lpType,
//		subvalue,			//LPBYTE  lpData,
//		cbData				//LPDWORD lpcbData
//	);
//
//	return (res == ERROR_SUCCESS);
//}
//
//BOOL deleteRegistryValue(const TCHAR *subkey, HKEY *key) {
//	LSTATUS res = RegDeleteValue(
//		*key,
//		subkey
//	);
//
//	return (res == ERROR_SUCCESS);
//}
