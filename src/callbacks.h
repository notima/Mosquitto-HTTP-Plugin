#ifndef MOSQ_HTTP_CALLBACKS_H
#define MOSQ_HTTP_CALLBACKS_H

#include <mosquitto_broker.h>

/**
 * Called by mosquitto when a client tries to connect to the broker.
 * The connection will be blocket unless this method returns MOSQ_ERR_SUCCESS.
 */
int cb_basic_auth(int eventType, void *event_data, void *userdata);

/**
 * Called by mosquitto when an access control is made. Access controls are made
 * when a client tres to read or write to the broker.
 * Unless MOSQ_ERR_SUCCESS is returned, the access control will be denied and 
 * read/write operations will be blocked.
 */
int cb_acl_check(int eventType, void *event_data, void *userdata);

#endif