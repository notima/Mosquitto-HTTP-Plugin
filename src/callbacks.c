#include <acl_cache.h>
#include <callbacks.h>
#include <denial_limit.h>
#include <config.h>
#include <http_client.h>
#include <json.h>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <stdio.h>
#include <stdlib.h>

int cb_basic_auth(int eventType, void *event_data, void *userdata) {
    struct mosquitto_evt_basic_auth *event = event_data;

    // Escaped, so a quote in the username or password can't add or override keys
    // (e.g. a second "username" that the backend would read instead of the first).
    char *username = json_string(event->username);
    char *password = json_string(event->password);
    // The client's address lets the backend count failed logins per address, so that
    // someone guessing passwords can't lock out the equipment that uses the account.
    char *address = json_string(mosquitto_client_address(event->client));
    char *authJson = NULL;
    if(username != NULL && password != NULL && address != NULL) {
        authJson = format_alloc(
            "{\"username\":%s,\"password\":%s,\"address\":%s}",
            username,
            password,
            address
        );
    }
    free(username);
    free(password);
    free(address);
    if(authJson == NULL) {
        return MOSQ_ERR_NOMEM;
    }

    int status = post(config_connect_uri, authJson);
    free(authJson);
    if(status < 0) {
        return MOSQ_ERR_NO_CONN;
    }
    mosquitto_log_printf(MOSQ_LOG_INFO, "HTTP STATUS: %d", status);
    if(status / 100 == 2) {
        mosquitto_log_printf(MOSQ_LOG_INFO, "Connection access granted for user: %s", event->username);
        return MOSQ_ERR_SUCCESS;
    } else if(status == 429) {
        mosquitto_log_printf(
            MOSQ_LOG_NOTICE,
            "Connection refused for user: %s from %s: too many failed logins",
            event->username,
            mosquitto_client_address(event->client)
        );
        return MOSQ_ERR_AUTH;
    } else {
        mosquitto_log_printf(
            MOSQ_LOG_INFO,
            "Connection access denied for user: %s from %s",
            event->username,
            mosquitto_client_address(event->client)
        );
        return MOSQ_ERR_AUTH;
    }
}

int cb_acl_check(int eventType, void *event_data, void *userdata) {
    struct mosquitto_evt_acl_check *event = event_data;
    const char *client_username = mosquitto_client_username(event->client);
    time_t now = monotonic_seconds();

    int cached = acl_cache_get(client_username, event->topic, event->access, now);
    if(cached == ACL_CACHE_ALLOWED) {
        return MOSQ_ERR_SUCCESS;
    }
    if(cached == ACL_CACHE_DENIED) {
        return MOSQ_ERR_ACL_DENIED;
    }
    // Over the limit: denied without asking, so that the client can't keep the broker
    // busy until it has been disconnected.
    if(denial_limit_exceeded(client_username, now)) {
        denial_limit_queue_kick(mosquitto_client_id(event->client));
        return MOSQ_ERR_ACL_DENIED;
    }

    // Escaped for the same reason as in cb_basic_auth: the topic is client-supplied too.
    char *username = json_string(client_username);
    char *topic = json_string(event->topic);
    char *aclJson = NULL;
    if(username != NULL && topic != NULL) {
        aclJson = format_alloc(
            "{\"username\":%s,\"topic\":%s,\"access\":\"%d\"}",
            username,
            topic,
            event->access
        );
    }
    free(username);
    free(topic);
    if(aclJson == NULL) {
        return MOSQ_ERR_NOMEM;
    }

    int status = post(config_acl_uri, aclJson);
    free(aclJson);
    if(status < 0) {
        return MOSQ_ERR_NO_CONN;
    }
    int allowed = status / 100 == 2;
    acl_cache_put(client_username, event->topic, event->access, allowed, now);
    if(allowed) {
        return MOSQ_ERR_SUCCESS;
    }

    mosquitto_log_printf(MOSQ_LOG_INFO, "Access denied for user: %s to topic %s", client_username, event->topic);
    if(denial_limit_record(client_username, now)) {
        mosquitto_log_printf(
            MOSQ_LOG_NOTICE,
            "User %s had more than %d denied access checks in a minute: denying its uncached checks for the rest of the minute",
            client_username,
            config_acl_max_denials_per_minute
        );
        denial_limit_queue_kick(mosquitto_client_id(event->client));
    }
    return MOSQ_ERR_ACL_DENIED;
}

int cb_tick(int eventType, void *event_data, void *userdata) {
    static time_t last_sweep = 0;

    denial_limit_kick_pending();

    time_t now = monotonic_seconds();
    if(now - last_sweep >= 60) {
        acl_cache_sweep(now);
        denial_limit_sweep(now);
        last_sweep = now;
    }
    return MOSQ_ERR_SUCCESS;
}
