/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: dfxfragment.h
 * Author: Ken Sheedlo
 *
 * Headers for a simple fragment class.
 *
 ******************************************************************************/

#ifndef DFXFRAGMENT_H
#define DFXFRAGMENT_H

#define GL_GLEXT_PROTOTYPES

#if defined(__APPLE__) || defined(MACOSX)
    #include<GLUT/glut.h>
    #include<OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include "octree.h"

/*Magic Numbers */
/* Spin rate in free-fall, in degrees per second. */
#define SPIN_RATE   720.0

/* Computed based on the radius of a tetrahedron of unit side length. */
#define BOUNDING_RADIUS 0.612

typedef struct {
    GLdouble color[4];
    GLdouble position[3];
    GLdouble rotv[3];
    GLdouble vel[3];
    GLdouble theta;
    GLdouble scale;
} dfx_fragment;

void dfx_fragment_init(dfx_fragment *fragment);

void dfx_fragment_set_pos(dfx_fragment *fragment, GLdouble vec[3]);

void dfx_fragment_set_color3d(dfx_fragment *fragment, GLdouble vec[3]);

void dfx_fragment_set_color4d(dfx_fragment *fragment, GLdouble vec[4]);

void dfx_fragment_rotate(dfx_fragment *fragment, GLdouble theta);

void dfx_fragment_set_scale(dfx_fragment *fragment, GLdouble scale);

void dfx_fragment_set_rotv(dfx_fragment *fragment, GLdouble rot[3]);

void dfx_fragment_set_vel(dfx_fragment *fragment, GLdouble vel[3]);

void dfx_fragment_step(dfx_fragment *fragment, const double dt, const double gravity);

void dfx_fragment_draw(dfx_fragment *fragment);

void dfx_fragment_get_bounds(bounding_box *box, const void *data);

void dfx_fragment_init_octree_vol(octree_vol *vol, dfx_fragment *fragment);

gdsl_element_t dfx_fragment_copy_alloc(void *data);

void dfx_fragment_copy_free(gdsl_element_t elt);

#endif
