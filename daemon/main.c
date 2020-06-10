/* Copyright 2020 PaX. */
/* This file is part of the SS13d (Space Station 13 daemon).
 * SS13d is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the 
 * License, or (at your option) any later version.
 * 
 * SS13d is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public 
 * License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public 
 * License along with SS13d. If not, see <https://www.gnu.org/licenses/>. */

 #define INITLINECNT 64
 
 #include <fcntl.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <unistd.h>
 
struct argstruct {
	char* configpath;
	char* serverroot;
	char* aroot;
	char* broot;
};

void usage(void);
int parseconfig(FILE* configfd, struct argstruct* returnstruct);

int main(int argc, char *argv[]) {
	struct argstruct* args = malloc(sizeof(args));
	if (argstruct == NULL) {
		fprintf(stderr, "argstruct malloc() failed.\n");
		return EXIT_FAILURE;
	}
	
	opterr = 0;
	
	while ((int arg = getopt(argc, argv, "+hc:")) != -1) {
		switch (arg) {
			case 'h':
				usage();
				return EXIT_SUCCESS;
			case 'c':
				args->configpath = optarg;
				break;
			default:
				fprintf(stderr, "Invalid argument.\n");
				return EXIT_FAILURE;
				break;
			}
		}

	if (access(argstruct->configpath, F_OK) != 0) {
		fprintf(stderr, "Configuration file specified does not exist.\n");
		return EXIT_FAILURE;
	}
	FILE* config = open(argstruct->configpath, O_RDONLY);
	if (config == -1) {
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
	char*** lineptr[] = malloc(sizeof(char*) * linebufcnt);
	if (lineptr == NULL) {
		fprintf(stderr, "Could not malloc line array.\n");
		return 1;
	}
	memset(lineptr, 0, sizeof(char*) * linebufcnt));
	unsigned int linepos = 0;
	size_t linesize = 0;
	
	while (linesize >= 0) {
		/* This is a great addition to a libc and I'm happy it's in POSIX. */
		getline(lineptr[linepos], &linesize, configfd);
		
		
		if (linebufcnt == linepos) {
			lineptr = realloc(lineptr, sizeof(char*) * linebufcnt + INITLINECNT);
			if (lineptr == NULL) {
				fprintf(stderr, "Ran out of space in line array and could not realloc.\n");
				return 1;
			}
			memset(lineptr + sizeof(char*) * linebufcnt, 0);
			linebufcnt = linebufcnt + INITLINECNT;
		}
	}
	assert(false);
	return 0;
}
void usage(void) {
	printf("-h: Print usage.\
			-c: Specify configuration file location.");
}
