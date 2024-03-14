#include "aviao.h"

void exit_aviao(Config *);

int _tmain(int argc, TCHAR **argv, TCHAR **envp) {
#ifdef UNICODE
	int i = _setmode(_fileno(stdin), _O_WTEXT);
	i = _setmode(_fileno(stdout), _O_WTEXT);
	i = _setmode(_fileno(stderr), _O_WTEXT);
#endif
	Config *cfg = malloc(sizeof(Config));
	if (cfg == NULL)
		return -1;

	if (!init_config(cfg)) {
		cout("Could not initialize configuration.\n");
		exit_aviao(cfg);
		return -1;
	}

	if (argc != 4) {
		cout("Wrong number of arguments.\nTry '%s [MAX_CAPACITY] [VELOCITY] [AIRPORT_ID]'.\n", argv[0]);
		exit_aviao(cfg);
		return -1;
	}

	cfg->airplane.max_capacity = _ttoi(argv[1]);
	cfg->airplane.velocity = _ttoi(argv[2]);
	cfg->airplane.airport_start.id = _ttoi(argv[3]);
	cfg->airplane.pid = GetProcessId(GetCurrentProcess());

	if (cfg->airplane.max_capacity < 1 || cfg->airplane.velocity < 1
		|| cfg->airplane.airport_start.id < 1) {
		cout("Invalid arguments.\nArguments must be positive numbers.\n");
		exit_aviao(cfg);
		return -1;
	}

	init_aviao(cfg);

	exit_aviao(cfg);
	return 0;
}

void exit_aviao(Config *cfg) {
	end_config(cfg);
	free(cfg);
	cfg = NULL;
}
