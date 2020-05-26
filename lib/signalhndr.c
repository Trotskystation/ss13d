#include "command.h"
#include "signalhndr.h"
/* On raise of signal SIGUSR1 from SS13d, sighandler() puts the
 * requested command from SS13d into the command list.
 * sival_ptr contains a pointer to memory mmap()ed as anonymous shared
 * memory before the fork() and execve() of DreamDaemon that contains
 * command data.
 * Reentrancy be damned?? */
void sighandler(int signal, siginfo_t* info, void* ucontext) {
	/* Exit if signal does not have data from sigqueue(). */
	if (info->si_code != SI_QUEUE) {
		return;
	}
	/* Check if SS13d has sent a reply address. Assign it and return
	 * if so. */
	if (info->si_value.sival_ptr->command == CMDREPLYADDRASSN) {
		ss13d_reply = info->si_value.sival_ptr->option.replyaddr;
		return;
	}
	/* Initialize list if head ptr is NULL. */
	if (command_list_head == NULL) {
		command_list_head = malloc(sizeof(command_list_e));
		command_list_tail = command_list_head;
		if (command_list_head == NULL) {
			return;
		}
		memset(command_list_tail, 0, sizeof(command_list_e));
	}
	/* If tail (OwO) element data ptr is NULL, copy data directly without
	 * allocating a new element. This should only happen if queue was
	 * just initialized. */
	 if (command_list_tail->data == NULL) {
		 command_list_tail->data = malloc(sizeof(sig_ipc_data));
		 if (command_list_tail->data == NULL) {
			 return;
		 }
		 memcpy(command_list_tail->data, info->si_value.sival_ptr, sizeof(sig_ipc_data));
		 return;
	 }
	/* Otherwise, allocate new element and copy data to it. */
	command_list_tail->next = malloc(sizeof(command_list_e));
	if (command_list_tail->next == NULL) {
		return;
	}
	command_list_tail = command_list_tail->next;
	memset(command_list_tail, 0, sizeof(command_list_e));
	memcpy(command_list_tail->data, info->si_value.sival_ptr, sizeof(sig_ipc_data));
	return;
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

