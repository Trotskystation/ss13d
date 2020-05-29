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
#define MAXSTRLEN 64

#include <signal.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../shared/command.h"
#include "../shared/ipc.h"

/* The commands this library can handle */
static const char* APISTR[][MAXCMDCHARCNT] = {{STRCMDINULL}, {STRCMDIREPLYADDRASSN},
{STRCMDSHUTDOWN}, {STRCMDRESTART}, {STRCMDSERVUPDATE}, {STRCMDTESTMERGE},
{STRCMDPING}};
static const unsigned int CMDCNT = sizeof(APISTR) / sizeof(APISTR[0]);

/* Element of queue of commands from SS13d to execute. */
struct command_queue_e {
	unsigned int command;
	char* option_str;
	struct command_queue_e* next;
} command_queue_e;

/* Command queue tracking globals. */
struct command_queue_e* command_q_head = NULL;
struct command_queue_e* command_q_tail = NULL;

/* Reply address to SS13d. */
struct sig_ipc_data* ss13d_reply = NULL;

/* On raise of signal SIGUSR1 from SS13d, sighandler() puts the
 * requested command from SS13d into the command list.
 * sival_ptr contains a pointer to memory mmap()ed as anonymous shared
 * memory before the fork() and execve() of DreamDaemon that contains
 * command data.
 * This function should be reentrant. This is impossible with our
 * command queuing model but it shouldn't matter because sigaction()
 * masks SIGUSR1 while the handler is running. */
void sighandler(int signal, siginfo_t* info, void* ucontext) {
	/* Exit if signal does not have data from sigqueue(). */
	if (info->si_code != SI_QUEUE) {
		return;
	}
	
	struct sig_ipc_data* data = (struct sig_ipc_data*)(info->si_value.sival_ptr);
	
	/* Exit on null command. */
	if (data->command == CMDINULL) {
		return;
	}
	
	/* Check if SS13d has sent a reply address. Assign it and return
	 * if so. */
	if (data->command == CMDIREPLYADDRASSN) {
		ss13d_reply = data->option.replyaddr;
		return;
	}
	
	/* Initialize queue if head ptr is NULL and queue is empty. */
	if (command_q_head == NULL) {
		command_q_head = malloc(sizeof(command_queue_e));
		command_q_tail = command_q_head;
		if (command_q_head == NULL) {
			return;
		}
		memset(command_q_head, 0, sizeof(command_queue_e));
	}
	
	/* If tail (OwO) element command is null command, copy data directly without
	 * allocating a new element. This should only happen if queue was
	 * just initialized. */
	 if (command_q_tail->command == CMDINULL) {
		 command_q_tail->command = data->command;
		 strcpy(command_q_tail->option_str, data->option.str);
		 return;
	 }
	 
	/* Otherwise, allocate new element and copy data to it. */
	command_q_tail->next = malloc(sizeof(command_queue_e));
	if (command_q_tail->next == NULL) {
		return;
	}
	command_q_tail = command_q_tail->next;
	memset(command_q_tail, 0, sizeof(command_queue_e));
	command_q_tail->command = ((struct sig_ipc_data*)(info->si_value.sival_ptr))->command;
	strcpy(command_q_tail->option_str, data->option.str);
	return;
}

/* Send command to SS13d. Also, of course, we assume we are getting
 * properly null-terminated strings from whatever called this function.
 * Return vals: "0" -> Success.
 * 				"1" -> Failure. */
char* raise_caller_sig(char* command_str, char* option_str) {
	if (ss13d_reply == NULL) {
		return "1";
	}
	pid_t caller_pid = getppid();
	ss13d_reply->command = atoi(command_str);
	strcpy(ss13d_reply->option.str, option_str);
	if (kill(caller_pid, 10) != 0) {
		return "1";
	}
	return "0";
}

/* Return ptr to first command in queue. If none, returns ptr to empty 
 * string. */
char* pollcommand() {
	raise_caller_sig(STRCMDPING, NULL);
	if (command_q_head == NULL) {
		return "";
	}
	char* command = malloc(sizeof(char)*MAXSTRLEN);
	if (command == NULL) {
		return "";
	}
	if (command_q_head->command > CMDCNT-1) {
		strcpy(command, "");
		return command;
	}
	strcpy(command, *APISTR[command_q_head->command]);
	return command;
}

/* Return optional string from first command in queue. If none, returns
 * empty string. */
char* pollcommandstr(void) {
	raise_caller_sig(STRCMDPING, NULL);
	if (command_q_head->option_str == NULL) {
		return "";
	}
	char* option_str = malloc(sizeof(char)*MAXSTRLEN);
	if (option_str == NULL) {
		return "";
	}
	strcpy(option_str, command_q_head->option_str);
	return option_str;
}

/* Clear the first command in queue and free memory. SS13 must call this
 * after retrieving a command or after retrieving a command and a
 * command option str. */
void freecmd(char* command, char* option_str) {
	if (command != NULL) {
		free(command);
	}
	if (option_str != NULL) {
		free(option_str);
	}
	
	/* Mask SIGUSR1 so we avoid a data race with sighandler(). */
	sigset_t* sigusr1mask = malloc(sizeof(sigset_t));
	if (sigusr1mask == NULL) {
		return;
	}
	if (sigemptyset(sigusr1mask) != 0) {
		return;
	}
	if (sigaddset(sigusr1mask, SA_SIGINFO) != 0) {
		return;
	}
	if (sigprocmask(SIG_BLOCK, sigusr1mask, NULL) != 0) {
		return;
	}
	
	if (command_q_head == NULL) {
		return;
	}
	if (command_q_head->next != NULL) {
		struct command_queue_e* tmpfree = command_q_head;
		command_q_head = command_q_head->next;
		free(tmpfree);
	}
	free(command_q_head);
	command_q_head = NULL;
	command_q_tail = NULL;
	
	/* Handle SIGUSR1 again. */
	sigprocmask(SIG_UNBLOCK, sigusr1mask, NULL);
	free(sigusr1mask);
}

/* Register command signal handler. 
 * Return vals: "0" -> Success.
 * 				"1" -> Failure. */
char* reg_sig_handler(void) {
	struct sigaction sa_hd;
	memset(&sa_hd, 0, sizeof(sigaction));
	sa_hd.sa_sigaction = sighandler;
	sa_hd.sa_flags = SA_SIGINFO;
	if (sigaction(SIGUSR1, &sa_hd, NULL) == -1) {
		return "1";
	}
	return "0";
}
