#include <mosquitto_broker.h>
#include <mosquitto_plugin.h>
#include <mosquitto.h>
#include <callbacks.h>
#include <config.h>
#include <stdio.h>

#define SUCCESS 0

void setup_config(struct mosquitto_opt *options, int option_count) {
    for(int i = 0; i < option_count; i++) {
        mosquitto_log_printf(MOSQ_LOG_INFO, "found config: %s : %s", options[i].key, options[i].value);
        if(set_config(options[i].key, options[i].value) == CONFIG_UNKNOWN) {
            mosquitto_log_printf(MOSQ_LOG_INFO, "Unknown configuration: %s", options[i].key, options[i].value);
        }
    }
}

mosq_plugin_EXPORT int mosquitto_plugin_version(int supported_version_count, const int *supported_versions) {
    return MOSQ_PLUGIN_VERSION;
}

mosq_plugin_EXPORT int mosquitto_plugin_init(mosquitto_plugin_id_t *identifier, void **userdata, struct mosquitto_opt *options, int option_count) {
    setup_config(options, option_count);
    int result = 0;
    result += mosquitto_callback_register(identifier, MOSQ_EVT_BASIC_AUTH, cb_basic_auth, NULL, userdata);
    result += mosquitto_callback_register(identifier, MOSQ_EVT_ACL_CHECK, cb_acl_check, NULL, userdata);
    return result;
}

mosq_plugin_EXPORT int mosquitto_plugin_cleanup(void *userdata, struct mosquitto_opt *options, int option_count) {
    return SUCCESS;
}