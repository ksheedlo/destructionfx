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
F - Switch weapons
1 - Jump
Space - Shoot simulated gun
Arrow keys - Rotate and tilt the camera  

## Final Submission Update

### Project Retrospective

I created a collision detection engine using octrees and a cool visualizer for
it. It uses lighting, textures, particle effects and fog. It features
first-person navigation for a more immersive experience. I've made the particle
effects more dramatic than they were in the review update. You can now use a
bigger simulated gun to put a huge crater in the brick wall and send particles 
flying everywhere. I'm proud of the attention to detail that this project 
allowed me to make. Each tiny particle is lit correctly on each of its faces.
The fog increases gradually as you smash more and more blocks. The large gun
features a spherical collision detection query that I wrote on the octree
engine.

### Notes for Best Usage

I still haven't perfected the line intersection query. This means that it's
still best to shoot directly at the sides of a box. Collision detection
sometimes fails to trigger when shooting at the boxes at oblique angles. The
particle effects are most obvious with the RPG, which is now the default gun.

### Project Metrics

- 33 commits
- 2,702 lines of code
- 40+ hours of work
- Github: [destructionfx](https://github.com/ksheedlo/destructionfx)

