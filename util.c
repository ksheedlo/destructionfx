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

double randf(FILE *rng) {
    union _u_rng rgen;
    fread(&rgen, sizeof(rgen), 1, rng);
    rgen.int_val = (rgen.int_val & _RNG_LO_MASK) | _RNG_HI_MASK;
    return rgen.dbl_val - 1.0;
}

void nrandf(double *results, FILE *rng, size_t n) {
    union _u_rng *rgenp;
    if ((rgenp = malloc(n * sizeof(union _u_rng))) == NULL) {
        results[0] = nan("dvich");
        return;
    }
    fread(rgenp, sizeof(union _u_rng), n, rng);
    for (uint32_t i = 0; i < n; i++) {
        rgenp[i].int_val = (rgenp[i].int_val & _RNG_LO_MASK) | _RNG_HI_MASK;
        results[i] = rgenp[i].dbl_val - 1.0;
    }
}

void random_uvec(double *result, FILE *rng, size_t dim) {
    nrandf(result, rng, dim);
    unitize(result, dim);
}

void unitize(double *vec, size_t dim) {
    double sum = 0.0, norm;
    for (uint32_t i = 0; i < dim; i++) {
        sum += (vec[i] * vec[i]);
    }
    norm = sqrt(sum);
    for (uint32_t i = 0; i < dim; i++) {
        vec[i] /= norm;
    }
}
