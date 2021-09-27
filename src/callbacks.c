#include <callbacks.h>
#include <config.h>
#include <http_client.h>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <mosquitto_internal.h>
#include <stdio.h>

int cb_basic_auth(int eventType, struct mosquitto_evt_basic_auth *event, void *userdata) {
    char authJson[128];
    sprintf(authJson, "{\"username\":\"%s\",\"password\":\"%s\"}", event->username, event->password);
    int status = post(config_connect_uri, authJson);
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

int cb_acl_check(int eventType, struct mosquitto_evt_acl_check *event, void *userdata) {
    char aclJson[128];
    sprintf(aclJson, "{\"clientId\":\"$s\",\"username\":\"%s\",\"topic\":\"$s\"}", event->client->id, event->client->username, event->topic);
    int status = post(config_acl_uri, aclJson);
    if(status < 0) {
        return MOSQ_ERR_NO_CONN;
    }
    if(status / 100 == 2) {
        return MOSQ_ERR_SUCCESS;
    } else {
        mosquitto_log_printf(MOSQ_LOG_INFO, "Access denied for user: %s to topic %s", event->client->username, event->topic);
        return MOSQ_ERR_ACL_DENIED;
    }
}