# Mosquitto HTTP Plugin

This is a plugin for mosquitto v2.0 onwards. It allows remote authentication of clients and access control through an external REST API.

## Building the plugin

The plugin does not require access to the mosquitto source code in order to be compiled. simply run the Makefile

    make

The compiled plugin is now in `bin/plugin.so`.

## Using the plugin

To use the plugin, add the following to your mosquitto configuration file:

    allow_anonymous false
    plugin /path/to/plugin.so
    auth_opt_http_host example.com
    auth_opt_http_port 8080
    auth_opt_http_connect_uri /rest/endpoint/connect
    auth_opt_http_acl_uri /rest/endpoint/access-control

## Developing a REST endpoint compatible with the plugin

The plugin will send POST requests to the configured endpoints when a client authenticates and when ACL checks are made. The requests will look something like the following examples:

### Authentication

    POST /rest/endpoint/connect HTTP/1.1
    Host: example.com
    Content-Type: application/json
    Content-Length: 43

    {"username":"User123","password":"pass123"}

### ACL check

    POST /rest/endpoint/access-control HTTP/1.1
    Host: example.com
    Content-Type: application/json
    Content-Length: 67

    {"clientId":"client-123","username":"User123","topic":"mqtt/topic"}

Both of these endpoints simply need to respond with a `2XX` status in order to allow the authentication/check. any other status will result in connections or publications being blocket.