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
 #define AROOTPATH "aroot/"
 #define BROOTPATH "broot/"
 #define CFGROOTPATH "cfg/"
 
 #include <dirent.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/stat.h>
 #include <sys/types.h>
 #include <unistd.h>
 
struct argstruct {
	char* serverroot;
	char* aroot;
	char* broot;
	char* cfgroot;
	char* repo;
};

void usage(void);
DIR* openorinitdir(char* path);
char* concatpath(char* str1, char* str2);

int main(int argc, char *argv[]) {
	struct argstruct* args = malloc(sizeof(args));
	if (args == NULL) {
		fprintf(stderr, "argstruct malloc() failed.\n");
		return EXIT_FAILURE;
	}
	
	opterr = 0;
	
	int arg;
	while ((arg = getopt(argc, argv, "+hr:")) != -1) {
		switch (arg) {
			case 'h':
				usage();
				return EXIT_SUCCESS;
			case 'r':
				args->serverroot = optarg;
				break;
			default:
				fprintf(stderr, "Invalid argument.\n");
				return EXIT_FAILURE;
				break;
			}
		}
	
	DIR* root = openorinitdir(args->serverroot);
	if (root == NULL) {
		fprintf(stderr, "Could not open directory.");
	}
	char* arootstr = malloc(strlen(args->serverroot) + strlen(AROOTPATH) + 1);
	char* brootstr = malloc(strlen(args->serverroot) + strlen(BROOTPATH) + 1);
	char* cfgrootstr = malloc(strlen(args->serverroot) + strlen(CFGROOTPATH) + 1);
	if (arootstr == NULL || brootstr == NULL || cfgrootstr == NULL) {
		fprintf(stderr, "malloc() path strings failed.");
	}
	
}

DIR* openorinitdir(char* path) {
	DIR* pathfd = opendir(path);
	if (pathfd == NULL) {
		if (mkdir(path, 0711) == -1) {
			return NULL;
		}
		DIR* pathfd = opendir(path);
		if (pathfd == NULL) {
			return NULL;
		}
	}
	return pathfd;
}

char* concatpath(char* basepath, char* concatent) {
	size_t basepathlen = strlen(basepath);
	size_t concatentlen = strlen(concatent);
	if (basepath[basepathlen-1] == '/') {
		basepath = strcat(basepath, concatent);
	} else {
		basepath = strcat(basepath, "/");
		basepath = strcat(result, concatent);
	}
	return basepath;
}

void usage(void) {
	printf("-h: Print usage.\
-r: Specify server root directory. Use absolute paths.\
-g: Specify server git repository.");
}
