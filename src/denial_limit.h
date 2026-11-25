#ifndef MOSQ_HTTP_DENIAL_LIMIT_H
#define MOSQ_HTTP_DENIAL_LIMIT_H

#include <time.h>

/**
 * Whether this username has had more than config_acl_max_denials_per_minute denied access
 * checks that had to be asked of the HTTP server, in the current minute. While it has, its
 * checks that aren't cached should be denied without asking.
 */
int denial_limit_exceeded(const char *username, time_t now);

/**
 * Count a denied access check that had to be asked of the HTTP server.
 * Returns 1 if this denial took the username over the limit, otherwise 0.
 */
int denial_limit_record(const char *username, time_t now);

/**
 * Ask for this client to be disconnected. It happens on the broker's next tick
 * (denial_limit_kick_pending), since a client can't safely be disconnected while the
 * broker is in the middle of an access check for it.
 */
void denial_limit_queue_kick(const char *client_id);

/**
 * Disconnect the clients asked for with denial_limit_queue_kick.
 */
void denial_limit_kick_pending(void);

/**
 * Forget the counts of minutes that are over.
 */
void denial_limit_sweep(time_t now);

/**
 * Forget everything and free the memory.
 */
void denial_limit_clear(void);

#endif
