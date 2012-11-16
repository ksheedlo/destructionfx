# destructionfx

Destructible 3-D environments - CSCI 4229 Final Project
Ken Sheedlo
Fall 2012

destructionfx is a simulated voxel environment designed to facilitate creative
destruction. It features realistic, efficient collision detection and
destruction effects.

# Project Setup

The build environment uses the [GDSL](http://home.gna.org/gdsl/) data structures
library and [Check](http://check.sourceforge.net/), a unit testing framework for
C. A setup script is included to install dependencies into the local
environment. To run the initial setup:

    $ ./setup.sh

From there, you should be able to build and run the code.

    $ make
    $ ./smash


