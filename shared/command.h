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
 
#define MAXCMDCHARCNT 3
/* Internal commands cannot be polled for by SS13. */

/* API 1 */
/* Internal. Null command. */
#define CMDINULL 0
#define STRCMDINULL "0"

/* Internal. Assign SS13d reply address.
 * SS13d response: No effect.
 * SS13 response: libss13dipc will assign a reply address from signal data.*/
#define CMDIREPLYADDRASSN 1
#define STRCMDIREPLYADDRASSN "1"

/* Server shutdown.
 * SS13d: SS13d will send it back.
 * SS13: SS13 will shut down. */
#define CMDSHUTDOWN 2
#define STRCMDSHUTDOWN "2"

/* Server restart.
 * SS13d: SS13d will send CMDSHUTDOWN back and queue restart for when DD exits.
 * SS13: No effect. */
#define CMDRESTART 3
#define STRCMDRESTART "3"

/* Schedule server revision update from primary git repository.
 * SS13d: SS13d will do that action.
 * SS13: No effect. */
#define CMDSERVUPDATE 4
#define STRCMDSERVUPDATE "4"

/* Schedule testmerge. Includes commit id in option string.
 * SS13d: SS13d will do that action.
 * SS13: No effect. */
#define CMDTESTMERGE 5
#define STRCMDTESTMERGE "5"
