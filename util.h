/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: util.h
 * Author: Ken Sheedlo
 *
 * Utility functions and macros.
 *
 * ****************************************************************************/

#ifndef DFX_UTIL_H
#define DFX_UTIL_H

#include<stdlib.h>
#include<stdio.h>

#if defined(__APPLE__) || defined(MACOSX)
    #include<GLUT/glut.h>
    #include<OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#define check_fail_gl_error() _do_gl_error_check(__FILE__, __LINE__, 1)
#define check_warn_gl_error() _do_gl_error_check(__FILE__, __LINE__, 0)

void _do_gl_error_check(char *filename, int line, int fail);

#endif
