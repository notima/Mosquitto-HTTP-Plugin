CFLAGS = -fPIC
CFLAGS += -Isrc
CFLAGS += -Imosquitto/lib/
CFLAGS += -Imosquitto/include/
CFLAGS += -Imosquitto/deps/

compile: precompile config.o http_client.o callbacks.o json.o acl_cache.o denial_limit.o plugin.o
	gcc -shared -o bin/plugin.so bin/obj/plugin.o bin/obj/config.o bin/obj/callbacks.o bin/obj/http_client.o bin/obj/json.o bin/obj/acl_cache.o bin/obj/denial_limit.o
	
precompile:
	mkdir -p bin/obj

config.o:
	gcc $(CFLAGS) -c src/config.c -o bin/obj/config.o

http_client.o:
	gcc $(CFLAGS) -c src/http_client.c -o bin/obj/http_client.o

callbacks.o:
	gcc $(CFLAGS) -c src/callbacks.c -o bin/obj/callbacks.o

json.o:
	gcc $(CFLAGS) -c src/json.c -o bin/obj/json.o

acl_cache.o:
	gcc $(CFLAGS) -c src/acl_cache.c -o bin/obj/acl_cache.o

denial_limit.o:
	gcc $(CFLAGS) -c src/denial_limit.c -o bin/obj/denial_limit.o

plugin.o:
	gcc $(CFLAGS) -c src/plugin.c -o bin/obj/plugin.o

clean:
	rm -rf bin