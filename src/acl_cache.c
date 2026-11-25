#include <acl_cache.h>
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uthash.h>

// The broker asks for an access check for every message it receives and every message it
// delivers, and each one would otherwise be a blocking HTTP request. Remembering the
// answers for a short time makes regular traffic almost free. The price is that a change
// of access takes up to config_acl_cache_seconds to apply to clients already connected.

struct acl_entry {
    char *key;
    size_t key_length;
    int allowed;
    time_t expires;
    UT_hash_handle hh;
};

// Answers for a longer username and topic together, in bytes, aren't cached. Real topics
// are far shorter, but MQTT allows up to 65535 bytes each, and without this a client could
// make every cache entry that large. With it, an entry takes at most about 650 bytes.
#define MAX_CACHED_LENGTH 512

// uthash keeps entries in the order they were added, so the first one is the oldest.
static struct acl_entry *cache = NULL;

time_t monotonic_seconds(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec;
}

// The username's length comes first, so no username and topic pair can produce the same
// key as another (both are chosen by the client and may contain any character).
static char *make_key(const char *username, const char *topic, int access, size_t *length) {
    if(username == NULL) username = "";
    if(topic == NULL) topic = "";
    int size = snprintf(NULL, 0, "%d:%zu:%s%s", access, strlen(username), username, topic);
    if(size < 0) return NULL;
    char *key = malloc((size_t)size + 1);
    if(key == NULL) return NULL;
    snprintf(key, (size_t)size + 1, "%d:%zu:%s%s", access, strlen(username), username, topic);
    *length = (size_t)size;
    return key;
}

static int too_long(const char *username, const char *topic) {
    size_t length = 0;
    if(username != NULL) length += strlen(username);
    if(topic != NULL) length += strlen(topic);
    return length > MAX_CACHED_LENGTH;
}

static void remove_entry(struct acl_entry *entry) {
    HASH_DEL(cache, entry);
    free(entry->key);
    free(entry);
}

int acl_cache_get(const char *username, const char *topic, int access, time_t now) {
    if(config_acl_cache_seconds <= 0 || too_long(username, topic)) return ACL_CACHE_MISS;

    size_t length;
    char *key = make_key(username, topic, access, &length);
    if(key == NULL) return ACL_CACHE_MISS;
    struct acl_entry *entry;
    HASH_FIND(hh, cache, key, length, entry);
    free(key);

    if(entry == NULL) return ACL_CACHE_MISS;
    if(entry->expires <= now) {
        remove_entry(entry);
        return ACL_CACHE_MISS;
    }
    return entry->allowed ? ACL_CACHE_ALLOWED : ACL_CACHE_DENIED;
}

void acl_cache_put(const char *username, const char *topic, int access, int allowed, time_t now) {
    if(config_acl_cache_seconds <= 0 || config_acl_cache_size <= 0 || too_long(username, topic)) return;

    struct acl_entry *entry = malloc(sizeof(*entry));
    if(entry == NULL) return;
    entry->key = make_key(username, topic, access, &entry->key_length);
    if(entry->key == NULL) {
        free(entry);
        return;
    }
    entry->allowed = allowed;
    entry->expires = now + config_acl_cache_seconds;

    struct acl_entry *existing;
    HASH_FIND(hh, cache, entry->key, entry->key_length, existing);
    if(existing != NULL) {
        remove_entry(existing);
    }
    // Topics are chosen by clients, so without a limit a client could fill memory by
    // using a new topic each time.
    while(cache != NULL && HASH_COUNT(cache) >= (unsigned int)config_acl_cache_size) {
        remove_entry(cache);
    }
    HASH_ADD_KEYPTR(hh, cache, entry->key, entry->key_length, entry);
}

void acl_cache_sweep(time_t now) {
    struct acl_entry *entry, *next;
    HASH_ITER(hh, cache, entry, next) {
        if(entry->expires <= now) {
            remove_entry(entry);
        }
    }
}

void acl_cache_clear(void) {
    struct acl_entry *entry, *next;
    HASH_ITER(hh, cache, entry, next) {
        remove_entry(entry);
    }
}
