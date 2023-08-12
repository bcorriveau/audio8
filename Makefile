# Makefile for test

all: libcaudio8.so

clean:
	rm -f audio8.o libcaudio8.so

libcaudio8.so: audio8.o
	gcc -o libcaudio8.so audio8.o -lSDL2 -shared

audio8.o: audio8.c
	gcc -c -g -Wall -I/usr/include/SDL2 audio8.c -fpic

