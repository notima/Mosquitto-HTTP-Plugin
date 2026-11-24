#ifndef MOSQ_HTTP_JSON_H
#define MOSQ_HTTP_JSON_H

/**
 * Return `value` as a JSON string literal, quotes included, with every character that
 * JSON requires escaped (", \ and control characters). A NULL value becomes `null`.
 *
 * Client-supplied values (username, password, topic) must go through this before being
 * put into a request body, or a value containing a quote could add or override keys.
 *
 * The result is allocated with malloc and must be freed by the caller.
 * Returns NULL if memory could not be allocated.
 */
char *json_string(const char *value);

/**
 * Like sprintf, but allocates a buffer exactly large enough for the result.
 * The result must be freed by the caller. Returns NULL on failure.
 */
char *format_alloc(const char *format, ...);

#endif
