/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: util.h
 * Author: Ken Sheedlo
 *
 * Utility functions and macros.
 *
 * ****************************************************************************/

#include "util.h"

void _do_gl_error_check(char *filename, int line, int fail) {
    GLenum err_code;
    int err_pred = 0;
    while ((err_code = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "%s:%d GL error: %s\n", filename, line, 
            gluErrorString(err_code));
        err_pred |= err_code;
    }
    if (err_pred && fail) {
        exit(err_pred);
    }
}

void _error_check(char *filename, int line, char *msg, int fail) {
    fprintf(stderr, "%s:%d Error: %s\n", filename, line, msg);
    if (fail) {
        exit(1);
    }
}

void write_string_wi(char *str, int x, int y, void *font) {
    glWindowPos2i(x, y);
    for(char *p = str; *p; p++) {
        glutBitmapCharacter(font, *p);
    }
}
