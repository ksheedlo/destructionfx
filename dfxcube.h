/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: fxcube.h
 * Author: Ken Sheedlo
 *
 * ****************************************************************************/

#ifndef DFXCUBE_H
#define DFXCUBE_H

#include<stdint.h>
#include<string.h>

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

/* Flag definitions. */
#define DFX_CUBE_TEXTURED   1
#define DFX_CUBE_VISIBLE    2
#define DFX_CUBE_DIRTY      4
#define DFX_CUBE_DEFAULTS   DFX_CUBE_DIRTY

/* Magic numbers */
#define _DFX_CUBE_EPSILON   0.001

typedef struct {
    GLdouble position[3];
    GLdouble color[4];
    GLdouble size;
    unsigned int texture;
    int32_t flags;
} dfx_cube;

void dfx_cube_init(dfx_cube *cube, GLdouble size);

void dfx_cube_set_pos(dfx_cube *cube, GLdouble vec[3]);

void dfx_cube_set_color3d(dfx_cube *cube, GLdouble vec[3]);

void dfx_cube_set_color4d(dfx_cube *cube, GLdouble vec[4]);

void dfx_cube_set_texture(dfx_cube *cube, unsigned int texture);

void dfx_cube_set_flags(dfx_cube *cube, int32_t flags);

void dfx_cube_set_all(dfx_cube *cube, 
                      GLdouble pos[3], 
                      GLdouble color[4], 
                      unsigned int texture, 
                      int32_t flags
                      );

void _dfx_cube_2f_nop(GLfloat op1, GLfloat op2);

void dfx_cube_draw(dfx_cube *cube);

void dfx_cube_get_bounds(bounding_box *box, const void *data);

void dfx_cube_init_octree_vol(octree_vol *vol, dfx_cube *cube);

#endif
