#ifndef MOSQ_HTTP_ACL_CACHE_H
#define MOSQ_HTTP_ACL_CACHE_H

#include <time.h>

#define ACL_CACHE_MISS -1
#define ACL_CACHE_DENIED 0
#define ACL_CACHE_ALLOWED 1

/**
 * Seconds from a clock that only moves forwards, unlike the time of day.
 */
time_t monotonic_seconds(void);

/**
 * Look up the HTTP server's last answer for this username, topic and access.
 * Returns ACL_CACHE_ALLOWED, ACL_CACHE_DENIED, or ACL_CACHE_MISS if there is none from the
 * last config_acl_cache_seconds (or caching is off).
 */
int acl_cache_get(const char *username, const char *topic, int access, time_t now);

/**
 * Remember the HTTP server's answer for this username, topic and access.
 * When the cache is full, the oldest answer is forgotten. Answers for a username and topic
 * longer than 512 bytes together aren't remembered.
 */
void acl_cache_put(const char *username, const char *topic, int access, int allowed, time_t now);

/**
 * Forget the answers that have expired.
 */
void acl_cache_sweep(time_t now);

/**
 * Forget every answer and free the cache's memory.
 */
void acl_cache_clear(void);

#endif
