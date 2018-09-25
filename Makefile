all: main.o ext.o
	cc main.o ext.o -lglut -lGLEW -lGL $(shell pkg-config --libs ruby-2.3)

main.o: main.c
	cc -c main.c -o main.o -Wall -Wextra $(shell pkg-config --cflags ruby-2.3)

ext.o: ext.c
	cc -c ext.c -o ext.o -Wall -Wextra $(shell pkg-config --cflags ruby-2.3)
