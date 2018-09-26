LIBS = -lstdc++ -lglut -lGLEW -lGL $(shell pkg-config --libs ruby-2.3)
CFLAGS = -Wall -Wextra $(shell pkg-config --cflags ruby-2.3)

OBJS = main.o ext.o

all: $(OBJS)
	cc $(OBJS) $(LIBS)

clean:
	rm -f a.out $(OBJS)

main.o: main.cpp
	cc -c main.cpp -o main.o $(CFLAGS)

ext.o: ext.c
	cc -c ext.c -o ext.o $(CFLAGS)
