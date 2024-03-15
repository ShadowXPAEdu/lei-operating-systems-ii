#include "control.h"

void exit_control(Config *);

int WINAPI _tWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow) {
#ifdef UNICODE
	int i = _setmode(_fileno(stdin), _O_WTEXT);
	i = _setmode(_fileno(stdout), _O_WTEXT);
	i = _setmode(_fileno(stderr), _O_WTEXT);
#endif
	Config *cfg = malloc(sizeof(Config));
	if (cfg == NULL)
		return -1;

	if (!init_config2(cfg, hInst, nCmdShow)) {
		exit_control(cfg);
		return -1;
	}

	init_control(cfg);

	exit_control(cfg);
	return 0;
}

//int _tmain(int argc, TCHAR **argv, TCHAR **envp) {
//#ifdef UNICODE
//	int i = _setmode(_fileno(stdin), _O_WTEXT);
//	i = _setmode(_fileno(stdout), _O_WTEXT);
//	i = _setmode(_fileno(stderr), _O_WTEXT);
//#endif
//	Config *cfg = malloc(sizeof(Config));
//	if (cfg == NULL)
//		return -1;
//
//	if (!init_config(cfg)) {
//		exit_control(cfg);
//		return -1;
//	}
//
//	init_control(cfg);
//
//	exit_control(cfg);
//	return 0;
//}

void exit_control(Config *cfg) {
	end_config2(cfg);
	free(cfg);
	cfg = NULL;
}
