include config.mk

CFLAGS = -fPIC
CFLAGS += -Isrc
CFLAGS += -I$(MOSQUITTO_DIR)/lib/
CFLAGS += -I$(MOSQUITTO_DIR)/include/

compile: precompile config.o http_client.o callbacks.o plugin.o
	gcc -shared -o bin/plugin.so bin/obj/plugin.o bin/obj/config.o bin/obj/callbacks.o bin/obj/http_client.o
	
precompile:
	mkdir -p bin/obj

config.o:
	gcc $(CFLAGS) -c src/config.c -o bin/obj/config.o

http_client.o:
	gcc $(CFLAGS) -c src/http_client.c -o bin/obj/http_client.o

callbacks.o:
	gcc $(CFLAGS) -c src/callbacks.c -o bin/obj/callbacks.o

plugin.o:
	gcc $(CFLAGS) -c src/plugin.c -o bin/obj/plugin.o