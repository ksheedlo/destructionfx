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

#define GL_GLEXT_PROTOTYPES

#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<math.h>

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

#define FATAL_ERROR(msg)    _error_check(__FILE__, __LINE__, msg, 1);
#define WARN_ERROR(msg)     _error_check(__FILE__, __LINE__, msg, 0);

#define _RNG_HI_MASK 0x3ff0000000000000L
#define _RNG_LO_MASK    0xfffffffffffffL

union _u_rng {
    int64_t int_val;
    double dbl_val;
};

void _do_gl_error_check(char *filename, int line, int fail);

void _error_check(char *filename, int line, char *msg, int fail);

void write_string_wi(char *str, int x, int y, void *font);

void nrandf(double *results, size_t n);

void random_uvec(double *result, size_t dim);

void unitize(double *vec, size_t dim);

void scale_vector(double *vec, double scl, size_t dim);

#endif
