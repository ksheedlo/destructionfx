#ifndef KMCAM_H
#define KMCAM_H

#define GL_GLEXT_PROTOTYPES

#include<stdio.h>
#include<stdlib.h>
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

/* This is a slightly wrong approximation, useful to avoid dividing by
 * zero and related sorts of ugliness. */
#define KM_PI   3.1415927

#define RADIANS(x)  ((x)*KM_PI/180.0)
#define DEGREES(x)  ((x)*180.0/KM_PI)

typedef struct {
    GLdouble position[3];
    GLdouble center[3];
    GLdouble up[3];
} km_camera;

void kmcam_init(km_camera *cam);

/* Translates the camera along it's canonical unit vectors.
 *
 * For instance, calling this function with a vector in the +x direction always
 * moves the camera to the right, regardless of which direction it is looking.
 */
void kmcam_translate(km_camera *cam, GLdouble vec[3]);

/* Translates the camera in world space.
 *
 * Calling this function moves the camera with respect to the global coordinate
 * system. GL transformations are not observed, so take care to pass the correct
 * transformation.
 */
void kmcam_translate_world(km_camera *cam, GLdouble vec[3]);

/* Gets the camera's position in world space.
 *
 */
void kmcam_getpos_world(GLdouble result[3], km_camera *cam);

/* rotx, roty, rotz -- rotates the camera about it's own axes
 *
 * theta is in degrees. Fairly self-explanatory. Axes are relative to the camera
 * itself, rather than the world, as in kmcam_translate.
 */
void kmcam_roty(km_camera *cam, GLdouble theta);

void kmcam_rotx(km_camera *cam, GLdouble theta);

void kmcam_rotz(km_camera *cam, GLdouble theta);

/* Applies the appropriate transformations to the scene for the given camera.
 *
 * This takes into account the camera's parameters and transforms the scene
 * accordingly. It should be called with a fresh MODELVIEW matrix and before
 * rendering objects in the scene.
 */
void kmcam_place(km_camera *cam);

/* Linear algebra helper functions: _km_norm3d, _km_crossd, _km_general_rotate
 * 
 * FIXME: This is a well-studied problem. Evaluate options for third-party
 * subroutines (e.g., Atlas BLAS) to replace these and improve code clarity and
 * efficiency.
 *
 * Special note on _km_general_rotate: about is the unit vector specified to
 * rotate about. It MUST be a unit vector.
 */
void _km_norm3d(GLdouble result[3], GLdouble vec[3]);

void _km_crossd(GLdouble result[3], GLdouble lhs[3], GLdouble rhs[3]);

void _km_general_rotate(GLdouble result[3], GLdouble vec[3], 
                        GLdouble about[3], GLdouble theta);

#endif
