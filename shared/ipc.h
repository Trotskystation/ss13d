#include <stdbool.h>

struct sig_ipc_data;
/* We need a get a pointer to a location where we can reply to
 * SS13d but we also need to maintain type compatibility with BYOND. */
union option {
	struct sig_ipc_data* replyaddr;
	char* str;
};

/* Data we get at from si_value. */
struct sig_ipc_data {
	unsigned int command;
	union option option;
} sig_ipc_data;
