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
all: smash_raw test 

kmcam.o: kmcam.c kmcam.h

util.o: util.c util.h
	$(CC) $(CFLAGS) -c $^ 

octree.o: octree.c octree.h 
	$(CC) $(CFLAGS) $(GDSLFLAGS) -c $^

dfxcube.o: dfxcube.c dfxcube.h 
	$(CC) $(CFLAGS) $(GDSLFLAGS) -c $^

smash_raw: smash.c octree.o kmcam.o util.o dfxcube.o CSCIx229.a
	$(CC) $(CFLAGS) $(GDSLFLAGS) -o $@ $^ $(GLLIBS) $(GDSLLIBS)

test: test.c test.h octree.o
	$(CC) $(CFLAGS) $(CUNITFLAGS) $(GDSLFLAGS) -o $@ $^ $(GDSLLIBS) $(CUNITLIBS)

#  Generic compile rules
.c.o:
	gcc -c -O -Wall $<

#  Generic compile and link
%: %.c CSCIx229.a
	gcc -Wall -O3 -o $@ $^ $(GLLIBS)

#  Create archive (include glWindowPos here if you need it)
CSCIx229.a:fatal.o loadtexbmp.o print.o project.o errcheck.o object.o
	ar -rcs CSCIx229.a $^

.PHONY: clean
clean:
	rm -rf *.o *.h.gch *.dSYM smash_raw test 
