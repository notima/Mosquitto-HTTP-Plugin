#include <mosquitto_broker.h>

/**
 * Called by mosquitto when a client tries to connect to the broker.
 * The connection will be blocket unless this method returns MOSQ_ERR_SUCCESS.
 */
int cb_basic_auth(int eventType, struct mosquitto_evt_basic_auth *event, void *userdata);