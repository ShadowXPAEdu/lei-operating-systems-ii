#include "passag.h"

void exit_passag(Config *);

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
		exit_passag(cfg);
		return -1;
	}

	if (argc < 4) {
		cout("Wrong number of arguments.\nTry '%s [DEPARTURE_AIRPORT_ID] [LANDING_AIRPORT_ID] [PASSENGER_NAME] [(Optional) WAIT_TIME]'.\n", argv[0]);
		exit_passag(cfg);
		return -1;
	}

	cfg->passenger.airport.id = _ttoi(argv[1]);
	cfg->passenger.airport_end.id = _ttoi(argv[2]);
	_cpy(cfg->passenger.name, argv[3], MAX_NAME);
	if (argc > 4) {
		cfg->passenger.wait_time = _ttoi(argv[4]);
	} else {
		cfg->passenger.wait_time = INFINITE;
	}

	if (cfg->passenger.airport.id < 1 || cfg->passenger.airport_end.id < 1 || cfg->passenger.wait_time < 1) {
		cout("Invalid arguments.\nArguments must be positive numbers.\n");
		exit_passag(cfg);
		return -1;
	}

	init_passag(cfg);

	exit_passag(cfg);
	return 0;
}

void exit_passag(Config *cfg) {
	end_config(cfg);
	free(cfg);
	cfg = NULL;
}
