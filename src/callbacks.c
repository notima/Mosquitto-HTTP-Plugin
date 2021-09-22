#include <callbacks.h>
#include <http_client.h>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <stdio.h>

#define AUTH_SERVER_ENDPOINT "/post"

void log_error(int status) {
}

int cb_basic_auth(int eventType, struct mosquitto_evt_basic_auth *event, void *userdata) {
    char authJson[128];
    sprintf(authJson, "{\"username\":\"%s\",\"password\":\"%s\"}", event->username, event->password);
    int status = post(AUTH_SERVER_ENDPOINT, authJson);
    if(status < 0) {
        return MOSQ_ERR_NO_CONN;
    }
    mosquitto_log_printf(MOSQ_LOG_INFO, "HTTP STATUS: %d", status);
    if(status / 100 == 2) {
        mosquitto_log_printf(MOSQ_LOG_INFO, "Connection access granted for user: %s", event->username);
        return MOSQ_ERR_SUCCESS;
    } else {
        mosquitto_log_printf(MOSQ_LOG_INFO, "Connection access denied for user: %s", event->username);
        return MOSQ_ERR_AUTH;
    }
}