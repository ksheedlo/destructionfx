CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter -std=gnu99 -g
GLLIBS=

SYSNAME=$(shell uname -s)

ifeq ($(SYSNAME),Darwin)
    GLLIBS += -framework GLUT -framework OpenGL
else
    GLLIBS += -lglut -lGLU
endif

GDSLFLAGS=$(shell ./env/bin/gdsl-config --cflags)
GDSLLIBS=$(shell ./env/bin/gdsl-config --libs)

CUNITFLAGS=$(shell pkg-config ./env/lib/pkgconfig/cunit.pc --cflags)
CUNITLIBS=$(shell pkg-config ./env/lib/pkgconfig/cunit.pc --libs)

.PHONY: all
all: smash test 

kmcam.o: kmcam.c kmcam.h

util.o: util.c util.h
	$(CC) $(CFLAGS) -c $^ $(GLLIBS)

octree.o: octree.c octree.h 
	$(CC) $(CFLAGS) $(GDSLFLAGS) -c $^

smash: smash.c octree.o kmcam.o util.o
	$(CC) $(CFLAGS) $(GDSLFLAGS) -o $@ $^ $(GLLIBS) $(GDSLLIBS)

test: test.c test.h octree.o
	$(CC) $(CFLAGS) $(CUNITFLAGS) $(GDSLFLAGS) -o $@ $^ -static $(GDSLLIBS) $(CUNITLIBS)

.PHONY: clean
clean:
	rm -rf *.o *.h.gch *.dSYM smash test 
