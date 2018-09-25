all:
	cc main.c -Wall -Wextra -lglut -lGLEW -lGL $(shell pkg-config --cflags --libs ruby-2.3)
