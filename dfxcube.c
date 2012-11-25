/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: fxcube.h
 * Author: Ken Sheedlo
 *
 * Voxel cube class.
 *
 * ****************************************************************************/

#include "dfxcube.h"

void dfx_cube_init(dfx_cube *cube, GLdouble size) {
    /* Set at the origin */
    cube->position[0] = cube->position[1] = cube->position[2] = 0.0;

    /* Color to white, no transparency */
    cube->color[0] = cube->color[1] = cube->color[2] = cube->color[3] = 1.0;

    cube->size = size;

    /* Don't use a texture */
    cube->texture = 0;
    cube->flags = DFX_CUBE_DEFAULTS;
}

void dfx_cube_set_pos(dfx_cube *cube, GLdouble vec[3]) {
    memcpy(cube->position, vec, 3 * sizeof(GLdouble));
}

void dfx_cube_set_color3d(dfx_cube *cube, GLdouble vec[3]) {
    memcpy(cube->color, vec, 3 * sizeof(GLdouble));
    cube->color[3] = 1.0;
}

void dfx_cube_set_color4d(dfx_cube *cube, GLdouble vec[4]) {
    memcpy(cube->color, vec, 4 * sizeof(GLdouble));
}

void dfx_cube_set_texture(dfx_cube *cube, unsigned int texture) {
    cube->texture = texture;
    dfx_cube_set_flags(cube, cube->flags | DFX_CUBE_TEXTURED);
}

void dfx_cube_set_flags(dfx_cube *cube, int32_t flags) {
    cube->flags = flags;
}

void dfx_cube_set_all(dfx_cube *cube,
                      GLdouble pos[3],
                      GLdouble color[4],
                      unsigned int texture,
                      int32_t flags
                      ) {
    dfx_cube_set_pos(cube, pos);
    dfx_cube_set_color4d(cube, color);
    cube->texture = texture;
    cube->flags = flags;
}

void _dfx_cube_2f_nop(GLfloat op1, GLfloat op2) {
    
}

void dfx_cube_draw(dfx_cube *cube) {
    void (*texfunc)(GLfloat, GLfloat);
    
    if (cube->flags & DFX_CUBE_TEXTURED) {
        texfunc = glTexCoord2f;
        glEnable(GL_TEXTURE_2D);
    } else {
        texfunc = _dfx_cube_2f_nop;
    }
    glColor4dv(cube->color);
    glPushMatrix();
    
    glTranslated(cube->position[0], cube->position[1], cube->position[2]);
    glScaled(cube->size, cube->size, cube->size);

    glBegin(GL_QUADS);
        /* Front face */
        glNormal3f(0, 0, 1);
        texfunc(0.0f, 0.0f);    glVertex3f(0.0, 0.0, 0.0);
        texfunc(0.0f, 1.0f);    glVertex3f(0.0, 1.0, 0.0);
        texfunc(1.0f, 1.0f);    glVertex3f(1.0, 1.0, 0.0);
        texfunc(1.0f, 0.0f);    glVertex3f(1.0, 0.0, 0.0);

        /* Back face */
        glNormal3f(0, 0, -1);
        texfunc(0.0f, 0.0f);    glVertex3f(1.0, 0.0, -1.0);
        texfunc(0.0f, 1.0f);    glVertex3f(1.0, 1.0, -1.0);
        texfunc(1.0f, 1.0f);    glVertex3f(0.0, 1.0, -1.0);
        texfunc(1.0f, 0.0f);    glVertex3f(0.0, 0.0, -1.0);

        /* Sides */
        glNormal3f(-1, 0, 0);
        texfunc(0.0f, 0.0f);    glVertex3f(0.0, 0.0, -1.0);
        texfunc(0.0f, 1.0f);    glVertex3f(0.0, 1.0, -1.0);
        texfunc(1.0f, 1.0f);    glVertex3f(0.0, 1.0, 0.0);
        texfunc(1.0f, 0.0f);    glVertex3f(0.0, 0.0, 0.0);

        glNormal3f(1, 0, 0);    
        texfunc(0.0f, 0.0f);    glVertex3f(1, 0, 0);
        texfunc(0.0f, 1.0f);    glVertex3f(1, 1, 0);
        texfunc(1.0f, 1.0f);    glVertex3f(1, 1, -1);
        texfunc(1.0f, 0.0f);    glVertex3f(1, 0, -1);

        /* Top face */
        glNormal3f(0, 1, 0);
        texfunc(0.0f, 0.0f);    glVertex3f(0, 1, 0);
        texfunc(0.0f, 1.0f);    glVertex3f(0, 1, -1);
        texfunc(1.0f, 1.0f);    glVertex3f(1, 1, -1);
        texfunc(1.0f, 0.0f);    glVertex3f(1, 1, 0);

        /* Bottom face */
        glNormal3f(0, -1, 0);
        texfunc(0.0f, 0.0f);    glVertex3f(1, 0, -1);
        texfunc(0.0f, 1.0f);    glVertex3f(1, 0, 0);
        texfunc(1.0f, 1.0f);    glVertex3f(0, 0, 0);
        texfunc(1.0f, 0.0f);    glVertex3f(0, 0, -1);

    glEnd();

    if (cube->flags & DFX_CUBE_TEXTURED) {
        glDisable(GL_TEXTURE_2D);
    }

    glPopMatrix();
}

void dfx_cube_get_bounds(bounding_box *box, const void *data) {
    dfx_cube *cube = (dfx_cube *)data;
    box->max_y = cube->size + (double)(cube->position[1]) - _DFX_CUBE_EPSILON;
    box->min_y = (double)(cube->position[1]) + _DFX_CUBE_EPSILON;
    box->min_x = (double)(cube->position[0]) + _DFX_CUBE_EPSILON;
    box->max_x = cube->size + (double)(cube->position[0]) - _DFX_CUBE_EPSILON;
    box->max_z = cube->size + (double)(cube->position[2]) - _DFX_CUBE_EPSILON;
    box->min_z = (double)(cube->position[2]) + _DFX_CUBE_EPSILON;
}

void dfx_cube_init_octree_vol(octree_vol *vol, dfx_cube *cube) {
    vol->data = cube;
    vol->get_bounds = dfx_cube_get_bounds;
}
