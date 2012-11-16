CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter -std=gnu99 -g
LFLAGS=

SYSNAME=$(shell uname -s)

ifeq ($(SYSNAME),Darwin)
    LFLAGS += -framework GLUT -framework OpenGL
else
    LFLAGS += -lglut -lGLU
endif

CFLAGS += `./env/bin/gdsl-config --cflags`
LFLAGS += `./env/bin/gdsl-config --libs`

CHECKFLAGS=`pkg-config ./env/lib/pkgconfig/check.pc --cflags`
CHECKLIBS=`pkg-config ./env/lib/pkgconfig/check.pc --libs`

.PHONY: all
all: smash

octree.o: octree.c octree.h 
	$(CC) $(CFLAGS) -c $^

smash: smash.c octree.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

.PHONY: clean
clean:
	rm -rf *.o *.h.gch *.dSYM smash
