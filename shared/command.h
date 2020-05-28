/* Internal commands cannot be polled for by SS13. */

/* Internal. Null command. */
#define CMDINULL 0

/* Internal. Assign SS13d reply address.
 * SS13d response: No effect.
 * SS13 response: libss13dipc will assign a reply address from signal data.*/
#define CMDIREPLYADDRASSN 1

/* Server shutdown.
 * SS13d: SS13d will send it back.
 * SS13: SS13 will shut down. */
#define CMDSHUTDOWN 2

/* Server restart.
 * SS13d: SS13d will send CMDSHUTDOWN back and queue restart for when DD exits.
 * SS13: No effect. */
#define CMDRESTART 3

/* Schedule server revision update from primary git repository.
 * SS13d: SS13d will do that action.
 * SS13: No effect. */
#define CMDSERVUPDATE 4

/* Schedule testmerge. Includes commit id in option_str.
 * SS13d: SS13d will do that action.
 * SS13: No effect. */
#define CMDTESTMERGE 5
