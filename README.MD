# Mosquitto HTTP Plugin

This is a plugin for mosquitto v2.0 onwards. It allows remote authentication of clients and access control through an external REST API.

## Building the plugin

The plugin requires a copy of the Mosquitto source code in order to be compiled. This is vendored as the `mosquitto` git submodule, pinned to the version matching the `eclipse-mosquitto` Docker image this project uses.

    # Fetch the vendored Mosquitto source code
    git submodule update --init mosquitto

The compiler needs to know where the source code is. The path is retrieved from a variable called `MOSQUITTO_DIR` which should be defined in a file called `config.mk`. Copy [config.mk.example](config.mk.example) to `config.mk` (it already points `MOSQUITTO_DIR` at the vendored submodule) and run the make file.

    make

The compiled plugin is now in `bin/plugin.so`.

### Dependencies

Before building the plugin, the follwoing libraries/packages will have to be installed:

- libcjson-dev

## Using the plugin

To use the plugin, add the following to your mosquitto configuration file:

    allow_anonymous false
    plugin /path/to/plugin.so
    auth_opt_http_host example.com
    auth_opt_http_port 8080
    auth_opt_http_connect_uri /rest/endpoint/connect
    auth_opt_http_acl_uri /rest/endpoint/access-control

## Developing a REST API compatible with the plugin

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

Both of these endpoints simply need to respond with a `2XX` status in order to allow the authentication/check. any other status will result in requests to connect, read or write being blocket.