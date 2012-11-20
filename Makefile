CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter -std=gnu99 -g
LFLAGS=

SYSNAME=$(shell uname -s)

ifeq ($(SYSNAME),Darwin)
    LFLAGS += -framework GLUT -framework OpenGL
else
    LFLAGS += -lglut -lGLU
endif

GDSLFLAGS=$(shell ./env/bin/gdsl-config --cflags)
GDSLLIBS=$(shell ./env/bin/gdsl-config --libs)

CUNITFLAGS=$(shell pkg-config ./env/lib/pkgconfig/cunit.pc --cflags)
CUNITLIBS=$(shell pkg-config ./env/lib/pkgconfig/cunit.pc --libs)

.PHONY: all
all: smash

octree.o: octree.c octree.h 
	$(CC) $(CFLAGS) $(GDSLFLAGS) -c $^

smash: smash.c octree.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS) $(GDSLLIBS)

.PHONY: clean
clean:
	rm -rf *.o *.h.gch *.dSYM smash
