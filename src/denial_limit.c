#include <config.h>
#include <denial_limit.h>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <stdlib.h>
#include <string.h>
#include <uthash.h>

// The answer cache doesn't help against a client that keeps using new topics it may not
// use: every one of them is a blocking HTTP request. So the denials that needed one are
// counted per username, and once there are too many in a minute, that username's
// uncached checks are denied without asking for the rest of the minute, and its clients
// that cause them are disconnected. Counting per username rather than per client keeps
// a client from starting over by reconnecting with a new client id.

#define WINDOW_SECONDS 60

struct user_denials {
    char *username;
    time_t window_start;
    int count;
    UT_hash_handle hh;
};

struct pending_kick {
    char *client_id;
    UT_hash_handle hh;
};

static struct user_denials *users = NULL;
static struct pending_kick *kicks = NULL;

static struct user_denials *find_user(const char *username) {
    struct user_denials *user;
    HASH_FIND_STR(users, username, user);
    return user;
}

static void remove_user(struct user_denials *user) {
    HASH_DEL(users, user);
    free(user->username);
    free(user);
}

int denial_limit_exceeded(const char *username, time_t now) {
    if(config_acl_max_denials_per_minute <= 0 || username == NULL) return 0;
    struct user_denials *user = find_user(username);
    return user != NULL
        && now - user->window_start < WINDOW_SECONDS
        && user->count > config_acl_max_denials_per_minute;
}

int denial_limit_record(const char *username, time_t now) {
    if(config_acl_max_denials_per_minute <= 0 || username == NULL) return 0;

    struct user_denials *user = find_user(username);
    if(user == NULL) {
        user = calloc(1, sizeof(*user));
        if(user == NULL) return 0;
        user->username = strdup(username);
        if(user->username == NULL) {
            free(user);
            return 0;
        }
        user->window_start = now;
        HASH_ADD_KEYPTR(hh, users, user->username, strlen(user->username), user);
    }
    if(now - user->window_start >= WINDOW_SECONDS) {
        user->window_start = now;
        user->count = 0;
    }
    user->count++;
    return user->count == config_acl_max_denials_per_minute + 1;
}

void denial_limit_queue_kick(const char *client_id) {
    if(client_id == NULL) return;
    struct pending_kick *kick;
    HASH_FIND_STR(kicks, client_id, kick);
    if(kick != NULL) return;

    kick = malloc(sizeof(*kick));
    if(kick == NULL) return;
    kick->client_id = strdup(client_id);
    if(kick->client_id == NULL) {
        free(kick);
        return;
    }
    HASH_ADD_KEYPTR(hh, kicks, kick->client_id, strlen(kick->client_id), kick);
}

void denial_limit_kick_pending(void) {
    struct pending_kick *kick, *next;
    HASH_ITER(hh, kicks, kick, next) {
        HASH_DEL(kicks, kick);
        if(mosquitto_kick_client_by_clientid(kick->client_id, false) == MOSQ_ERR_SUCCESS) {
            mosquitto_log_printf(
                MOSQ_LOG_NOTICE,
                "Disconnected client %s: too many denied access checks",
                kick->client_id
            );
        }
        free(kick->client_id);
        free(kick);
    }
}

void denial_limit_sweep(time_t now) {
    struct user_denials *user, *next;
    HASH_ITER(hh, users, user, next) {
        if(now - user->window_start >= WINDOW_SECONDS) {
            remove_user(user);
        }
    }
}

void denial_limit_clear(void) {
    struct user_denials *user, *next_user;
    HASH_ITER(hh, users, user, next_user) {
        remove_user(user);
    }
    struct pending_kick *kick, *next_kick;
    HASH_ITER(hh, kicks, kick, next_kick) {
        HASH_DEL(kicks, kick);
        free(kick->client_id);
        free(kick);
    }
}
