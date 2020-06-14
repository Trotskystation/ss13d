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
 
 #define PROGIDENT "SS13d (Space Station 13 daemon)"
 #define DEFAULTSERVERROOT "ss13_root/"
 #define AROOTPATH "aroot/"
 #define BROOTPATH "broot/"
 #define CFGROOTPATH "cfg/"
 #define BYONDROOTPATH "byond/"
 
 #include <dirent.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/stat.h>
 #include <sys/types.h>
 #include <unistd.h>
 
struct argstruct {
	char** byondroot;
	char** serverroot;
	char** aroot;
	char** broot;
	char** cfgroot;
	char** repo;
};

void usage(void);
DIR* openorinitdir(char* path);
char* concatpath(char* str1, char* str2);
void exit_error(char* errstr);

int main(int argc, char *argv[]) {
	openlog(PROGIDENT, LOG_PID, LOG_DAEMON);
	
	struct argstruct* args = malloc(sizeof(args));
	if (args == NULL) {
		exit_error("argstruct malloc() failed.");
		return EXIT_FAILURE;
	}
	
	opterr = 0;
	
	int arg;
	char* uidarg = 0;
	char* gidarg = 0;
	while ((arg = getopt(argc, argv, "+hu:g:r:G:")) != -1) {
		switch (arg) {
			case 'h':
				usage();
				return EXIT_SUCCESS;
			case 'u':
				uidarg = optarg;
				break;
			case 'g':
				gidarg = optarg;
				break;
			case 'r':
				args->serverroot = optarg;
				break;
			case 'G':
				args->repo = optarg;
				break;
			default:
				fprintf(stderr, "Invalid argument. \"-u\" for usage information.");
				return EXIT_FAILURE;
				break;
			}
		}
		
	if (atoi(gidarg) != 0) {
		if (setgid(atoi(gidarg)) != 0) {
			exit_error("Could not change GID to specified!");
		}
		if (setuid(atoi(uidarg)) != 0) {
			exit_error("Could not change UID to specified!");
		}
	}
	
	if (getuid() == 0) {
		syslog(LOG_WARNING, "Warning: Running as root!");
	}
	
	if (args-serverroot != NULL) {
		DIR* root = openorinitdir(args->serverroot);
		if (root == NULL) {
			exit_error( "Could not open directory.");
		}
		char* arootstr = malloc(strlen(args->serverroot) + strlen(AROOTPATH) + 1);
		char* brootstr = malloc(strlen(args->serverroot) + strlen(BROOTPATH) + 1);
		char* cfgrootstr = malloc(strlen(args->serverroot) + strlen(CFGROOTPATH) + 1);
		char* byondrootstr = malloc(strlen(args->serverroot) + strlen(BYONDROOTPATH) + 1);
		
		if (arootstr == NULL || brootstr == NULL || cfgrootstr == NULL || byondrootstr == NULL ) {
			exit_error("malloc() path strings failed.");
		}
		arootstr = concatpath(arootstr, AROOTPATH);
		brootstr = concatpath(brootstr, BROOTPATH);
		cfgrootstr = concatpath(cfgrootstr, CFGROOTPATH);
		byondrootstr = concatpath(byondrootstr, CFGROOTPATH);
	}
	
}

DIR* initopendir(char* path) {
	if (mkdir(path, 0755) == -1) {
		return NULL;
	}
	DIR* pathfd = opendir(path);
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
	printf("-h: Print usage.\n\
-u: Optional, specify UID to drop privs to.\n\
-g: Optional, specify GID to drop privs to.\n\
-r: Optional, specify server root directory. Use absolute paths. If not used,\
you must specify a UID/GID and we will try to use the home directory. \n\
-G: Specify server git repository. Protocol: git://\n");
}

void exit_error(char* errstr) {
	fprintf(stderr, "%s\n", errstr);
	syslog(LOG_CRIT, "%s", errstr);
	closelog();
	exit(EXIT_FAILURE);
}
