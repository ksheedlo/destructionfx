/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: dfxfragment.h
 * Author: Ken Sheedlo
 *
 * Simple fragment class.
 *
 ******************************************************************************/

#include "dfxfragment.h"

void dfx_fragment_init(dfx_fragment *fragment) {
    fragment->position[0] = fragment->position[1] = fragment->position[2] = 0.0;

    fragment->rotv[0] = fragment->rotv[2] = 0.0;
    fragment->rotv[1] = 1.0;
    fragment->theta = 0.0;

    fragment->scale = 0.05;
    
    fragment->color[0] = 0.275;
    fragment->color[1] = 0.173;
    fragment->color[2] = 0.078;
    fragment->color[3] = 1.0;
}

void dfx_fragment_set_pos(dfx_fragment *fragment, GLdouble vec[3]) {
    memcpy(fragment->position, vec, 3*sizeof(GLdouble));
}

void dfx_fragment_set_color3d(dfx_fragment *fragment, GLdouble vec[3]) {
    memcpy(fragment->color, vec, 3*sizeof(GLdouble));
    fragment->color[3] = 1.0;
}

void dfx_fragment_set_color4d(dfx_fragment *fragment, GLdouble vec[4]) {
    memcpy(fragment->color, vec, 4*sizeof(GLdouble));
}

void dfx_fragment_rotate(dfx_fragment *fragment, GLdouble theta) {
    GLdouble ang = fragment->theta;
    fragment->theta = (ang + theta > 360.0) ? (ang + theta) - 360.0 : (ang + theta);
}

void dfx_fragment_set_scale(dfx_fragment *fragment, GLdouble scale) {
    fragment->scale = scale;
}

void dfx_fragment_set_rotv(dfx_fragment *fragment, GLdouble rot[3]) {
    memcpy(fragment->rotv, rot, 3*sizeof(GLdouble));
}

void dfx_fragment_draw(dfx_fragment *fragment) {

    glColor4f(fragment->color[0], fragment->color[1], fragment->color[2], fragment->color[3]);
    glPushMatrix();

    glTranslatef(fragment->position[0], fragment->position[1], fragment->position[2]);
    glRotatef(fragment->theta, fragment->rotv[0], fragment->rotv[1], fragment->rotv[2]);
    glScalef(fragment->scale, fragment->scale, fragment->scale);

    glTranslatef(-0.5, -0.204, -0.289);
    glBegin(GL_TRIANGLES);
        glNormal3f(0, -1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(1, 0, 0);
        glVertex3f(0.5, 0, 0.866);

        glNormal3f(0, 0.289, -0.817);
        glVertex3f(0, 0, 0);
        glVertex3f(1, 0, 0);
        glVertex3f(0.5, 0.817, 0.289);

        glNormal3f(-0.708, 0.289, 0.409);
        glVertex3f(0.5, 0, 0.866);
        glVertex3f(0, 0, 0);
        glVertex3f(0.5, 0.817, 0.289);

        glNormal3f(0.708, 0.289, 0.409);
        glVertex3f(1, 0, 0);
        glVertex3f(0.5, 0, 0.866);
        glVertex3f(0.5, 0.817, 0.289);
    glEnd();

    glPopMatrix();
}

void dfx_fragment_get_bounds(bounding_box *box, const void *data) {
    dfx_fragment *fragment = (dfx_fragment *)data;
    box->min_x = fragment->position[0] - (fragment->scale * BOUNDING_RADIUS);
    box->max_x = fragment->position[0] + (fragment->scale * BOUNDING_RADIUS);
    box->min_y = fragment->position[1] - (fragment->scale * BOUNDING_RADIUS);
    box->max_y = fragment->position[1] + (fragment->scale * BOUNDING_RADIUS);
    box->min_z = fragment->position[2] - (fragment->scale * BOUNDING_RADIUS);
    box->max_z = fragment->position[2] + (fragment->scale * BOUNDING_RADIUS);
}

void dfx_fragment_init_octree_vol(octree_vol *vol, dfx_fragment *fragment) {
    vol->data = fragment;
    vol->get_bounds = dfx_fragment_get_bounds;
}
