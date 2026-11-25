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
    plugin_opt_http_host example.com
    plugin_opt_http_port 8080
    plugin_opt_http_connect_uri /rest/endpoint/connect
    plugin_opt_http_acl_uri /rest/endpoint/access-control

These options are optional:

- `plugin_opt_acl_cache_seconds` (default `60`): how long an answer from the ACL endpoint is remembered, for one username, topic and kind of access. The broker checks access for every message it receives and every message it delivers, and while it waits for the endpoint, it serves no other client. A change of access takes up to this long to apply to clients already connected. `0` turns the cache off.
- `plugin_opt_acl_cache_size` (default `10000`): how many answers are remembered at most. When full, the oldest is forgotten first. Answers for a username and topic longer than 512 bytes together aren't cached, so an answer takes at most about 650 bytes, and a full cache of 10000 at most about 6.5 MB (typically about 160 bytes an answer).
- `plugin_opt_acl_max_denials_per_minute` (default `1000`): how many denied checks that had to be asked of the endpoint (not answered from the cache) one username may cause in a minute. Above it, that username's uncached checks are denied without asking for the rest of the minute, and its clients that cause them are disconnected. This stops a client from keeping the broker busy by using a new topic it may not use each time. `0` turns the limit off.

## Developing a REST API compatible with the plugin

The plugin will send POST requests to the configured endpoints when a client authenticates and when ACL checks are made. The requests will look something like the following examples:

### Authentication

    POST /rest/endpoint/connect HTTP/1.1
    Host: example.com
    Content-Type: application/json
    Content-Length: 66

    {"username":"User123","password":"pass123","address":"192.0.2.10"}

`address` is the client's IP address, so the API can count failed logins per address. If the API answers `429`, the connection is refused, and the broker logs that the user had too many failed logins.

### ACL check

    POST /rest/endpoint/access-control HTTP/1.1
    Host: example.com
    Content-Type: application/json
    Content-Length: 67

    {"clientId":"client-123","username":"User123","topic":"mqtt/topic"}

Both of these endpoints simply need to respond with a `2XX` status in order to allow the authentication/check. any other status will result in requests to connect, read or write being blocket.