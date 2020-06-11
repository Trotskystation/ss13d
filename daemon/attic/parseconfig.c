void main(void) {
	if (access(args->configpath, F_OK) != 0) {
		fprintf(stderr, "Configuration file specified does not exist.\n");
		return EXIT_FAILURE;
	}
	FILE* config = fopen(args->configpath, "r");
	if (config == NULL) {
		fprintf(stderr, "Could not open configuration file.\n");
		return EXIT_FAILURE;
	}
	
	int configerr = parseconfig(config, args);
	if (configerr != 0) {
		fprintf(stderr, "Failed to parse configuration file.\n");
		return EXIT_FAILURE;
	}
}

int parseconfig(FILE* configfd, struct argstruct* returnstruct) {
	unsigned int linebufcnt = INITLINECNT;
	char*** lineptr = malloc(sizeof(char*) * linebufcnt);
	if (lineptr == NULL) {
		fprintf(stderr, "Could not malloc line array.\n");
		return 1;
	}
	memset(lineptr, 0, sizeof(char*) * linebufcnt);
	unsigned int linei = 0;
	size_t linesize = 0;
	
	while (linesize > 0) {
		/* This is a great addition to a libc and I'm happy it's in POSIX. */
		if (getline(lineptr[linei], &linesize, configfd) == -1) {
			break;
		}
		if (linebufcnt == linei) {
			lineptr = realloc(lineptr, sizeof(char*) * linebufcnt + INITLINECNT);
			if (lineptr == NULL) {
				fprintf(stderr, "Ran out of space in line array and could not realloc.\n");
				return 1;
			}
			memset(lineptr + sizeof(char*) * linebufcnt, 0, sizeof(char*) * INITLINECNT);
			linebufcnt = linebufcnt + INITLINECNT;
		}
		linei++;
	}
	
	for(unsigned int i=0; i>=linei; i++) {
		if (*(lineptr[i])[0] == '#') {
			continue;
		}
		char* token = strtok(*lineptr[i], " ");
		/* May switch to a enum or something in the future. */
		if (strcmp(token, "SERVERROOT")) {
			
		}
	}
	return 0;
}
