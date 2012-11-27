# destructionfx

Destructible 3-D environments - CSCI 4229 Final Project  
Ken Sheedlo  
Fall 2012

destructionfx is a simulated voxel environment designed to facilitate creative
destruction. It features realistic, efficient collision detection and
destruction effects.

## Project Setup

The build environment uses the [GDSL](http://home.gna.org/gdsl/) data structures
library and [CUnit](http://cunit.sourceforge.net/), a unit testing framework for
C. A setup script is included to install dependencies into the local
environment. To run the initial setup:

    $ ./setup.sh

From there, you should be able to build and run the code.

    $ make
    $ ./smash

## Controls

WASD - Move
Space - Shoot invisible ray
Arrow keys - Rotate and tilt the camera

## Review Update

### Project Metrics

- 25 commits
- 2,475 lines of code
- 30+ hours of work
- Github: [destructionfx](https://github.com/ksheedlo/destructionfx)

### Notes for Best Usage

The line intersection test isn't perfect. It's best if you line up facing one of
the sides of the box and shoot directly at the brick wall. The particle effects
are best seen when shooting the corners of the structure.

### Stuff I'd Like to Do

There are a lot of ways I could go with this. I could play with fog, or shaders,
or expand on the particle effects I've already got. I'm probably going to try
and get the octree to detect spherical intersections to simulate area weapons.
Then I might try building the entire scene out of DFX cubes and having an entire
destructible environment.

