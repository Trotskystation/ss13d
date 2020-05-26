#include <signal.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../shared/command.h"
#include "../shared/ipc.h"
#include "
/* Element of queue of commands from SS13d to execute. */
typedef struct command_list_e {
	sig_ipc_data* data;
	struct command_list_e* next;
} command_list_e;

/* Command list tracking globals. */
command_list_e* command_list_head = NULL;
command_list_e* command_list_tail = NULL;

/* Reply address to SS13d. */
sig_ipc_data* ss13d_reply = NULL;



/* Check if there is a command in queue.
 * Return vals: "0" -> No.
 * 				"1" -> Yes. */
char* pollcommand(void) {
	
}

/* Send command to SS13d. 
 * Return vals: "0" -> Success.
 * 				"1" -> Failure. */
char* raise_caller_sig(char* command_str, char* option_str) {
	pid_t caller_pid = getppid();
	if (ss13d_reply == NULL) {
		return "1";
	}
	ss13d_reply->command = atoi(command_str);
	memcpy(ss13d_reply->option.str, option_str);
	if (kill(caller_pid, 10) != 0) {
		return "1";
	}
	return "0";
}

