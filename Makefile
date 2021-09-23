compile: precompile config.o http_client.o callbacks.o plugin.o
	gcc -shared -o bin/plugin.so bin/obj/plugin.o bin/obj/config.o bin/obj/callbacks.o bin/obj/http_client.o
	
precompile:
	mkdir -p bin/obj

config.o:
	gcc -fPIC -Isrc -c src/config.c -o bin/obj/config.o

http_client.o:
	gcc -fPIC -Isrc -c src/http_client.c -o bin/obj/http_client.o

callbacks.o:
	gcc -fPIC -Isrc/include -Isrc -c src/callbacks.c -o bin/obj/callbacks.o

plugin.o:
	gcc -Isrc/include -Isrc -c src/plugin.c -o bin/obj/plugin.o