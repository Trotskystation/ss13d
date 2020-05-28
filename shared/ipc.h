/* Copyright 2020 PaX */
/* This file is part of SS13d (Space Station 13 daemon).
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
