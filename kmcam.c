/* Ken Sheedlo
 * 3D camera utilities for OpenGL 
 */

#include "kmcam.h"

void kmcam_init(km_camera *cam) {
    /* Start at the origin */
    cam->position[0] = 0.0;
    cam->position[1] = 0.0;
    cam->position[2] = 0.0;

    /* Looking straight ahead */
    cam->center[0] = 0.0;
    cam->center[1] = 0.0;
    cam->center[2] = -1.0;

    /* Up vector along +y axis */
    cam->up[0] = 0.0;
    cam->up[1] = 1.0;
    cam->up[2] = 0.0;
}

void kmcam_translate(km_camera *cam, GLdouble vec[3]){
    GLdouble *pos = cam->position;
    GLdouble *center = cam->center;
    GLdouble z_vec[3], y_vec[3], x_vec[3];
    GLdouble tmp[3] = {
        center[0] - pos[0],
        center[1] - pos[1],
        center[2] - pos[2]
    };

    /* Determine axes relative to the camera as unit vectors. */
    _km_norm3d(z_vec, tmp);
    _km_norm3d(y_vec, cam->up);
    _km_crossd(tmp, z_vec, y_vec);
    _km_norm3d(x_vec, tmp);

    /* Determine new position from vec and the new axes just generated. */
    tmp[0] = pos[0] + (vec[0] * x_vec[0]) + (vec[1] * y_vec[0]) + (vec[2] * z_vec[0]);
    tmp[1] = pos[1] + (vec[0] * x_vec[1]) + (vec[1] * y_vec[1]) + (vec[2] * z_vec[1]);
    tmp[2] = pos[2] + (vec[0] * x_vec[2]) + (vec[1] * y_vec[2]) + (vec[2] * z_vec[2]);

    /* Set the new center point. */
    center[0] += (tmp[0] - pos[0]);
    center[1] += (tmp[1] - pos[1]);
    center[2] += (tmp[2] - pos[2]);

    /* Set the position. */
    pos[0] = tmp[0];
    pos[1] = tmp[1];
    pos[2] = tmp[2];
}

void kmcam_translate_world(km_camera *cam, GLdouble vec[3]) {
    /* Translate the camera's point position */
    cam->position[0] += vec[0];
    cam->position[1] += vec[1];
    cam->position[2] += vec[2];

    /* Translate the center point */
    cam->center[0] += vec[0];
    cam->center[1] += vec[1];
    cam->center[2] += vec[2];

}

void kmcam_getpos_world(GLdouble result[3], km_camera *cam) {
    result[0] = cam->position[0];
    result[1] = cam->position[1];
    result[2] = cam->position[2];
}

void kmcam_getpos_offset(GLdouble result[3], km_camera *cam, GLdouble vec[3]) {
    GLdouble tmp[3];

    /* Translate the camera, get it's position, translate back */
    kmcam_translate(cam, vec);
    kmcam_getpos_world(result, cam);
    for (int i = 0; i < 3; i++) {
        tmp[i] = -1.0*vec[i];
    }
    kmcam_translate(cam, tmp);
}

void kmcam_roty(km_camera *cam, GLdouble theta) {
    GLdouble *pos = cam->position;
    GLdouble *center = cam->center;
    GLdouble *up = cam->up;

    GLdouble dv[3] = {
        center[0] - pos[0],
        center[1] - pos[1],
        center[2] - pos[2]
    };
    GLdouble result[3];
    GLdouble about[3] = {
        0.0, 
        1.0,
        0.0
    };
    _km_general_rotate(result, dv, about, theta);

    center[0] = pos[0] + result[0];
    center[1] = pos[1] + result[1];
    center[2] = pos[2] + result[2];

    _km_general_rotate(result, up, about, theta);

    up[0] = result[0];
    up[1] = result[1];
    up[2] = result[2];
}

void kmcam_rotx(km_camera *cam, GLdouble theta) {
    GLdouble *pos = cam->position;
    GLdouble *center = cam->center;
    GLdouble *up = cam->up;

    GLdouble dv[3] = {
        center[0] - pos[0],
        center[1] - pos[1],
        center[2] - pos[2]
    };
    GLdouble about[3];
    GLdouble result[3];

    _km_crossd(about, dv, up);
    _km_norm3d(about, about);

    _km_general_rotate(result, dv, about, theta);

    center[0] = pos[0] + result[0];
    center[1] = pos[1] + result[1];
    center[2] = pos[2] + result[2];

    _km_general_rotate(result, up, about, theta);

    up[0] = result[0];
    up[1] = result[1];
    up[2] = result[2];
}

void kmcam_rotz(km_camera *cam, GLdouble theta) {
    GLdouble *pos = cam->position;
    GLdouble *center = cam->center;
    GLdouble *up = cam->up;

    GLdouble dv[3] = {
        center[0] - pos[0],
        center[1] - pos[1],
        center[2] - pos[2]
    };
    GLdouble about[3];
    GLdouble result[3];

    _km_norm3d(about, dv);
    _km_general_rotate(result, up, about, theta);

    up[0] = result[0];
    up[1] = result[1];
    up[2] = result[2];
}

void kmcam_place(km_camera *cam) {
    GLdouble *pos = cam->position;
    GLdouble *center = cam->center;
    GLdouble *up = cam->up;

    glMatrixMode(GL_MODELVIEW);
    gluLookAt(
            pos[0], pos[1], pos[2], 
            center[0], center[1], center[2], 
            up[0], up[1], up[2]
            );
}

void _km_norm3d(GLdouble result[3], GLdouble vec[3]) {
    GLdouble x = vec[0], y = vec[1], z = vec[2];
    GLdouble mag = sqrt(x*x + y*y + z*z);

    result[0] = x / mag;
    result[1] = y / mag;
    result[2] = z / mag;
}

void _km_crossd(GLdouble result[3], GLdouble lhs[3], GLdouble rhs[3]) {
    result[0] = lhs[1] * rhs[2] - rhs[1] * lhs[2];
    result[1] = lhs[0] * rhs[2] - rhs[0] * lhs[2];
    result[2] = lhs[0] * rhs[1] - rhs[0] * lhs[1];
}

void _km_general_rotate(GLdouble result[3], 
                        GLdouble vec[3], 
                        GLdouble about[3], 
                        GLdouble theta) {
    double one_minus_cos = 1.0 - cos(RADIANS(theta));
    double sin_theta = sin(RADIANS(theta));

    GLdouble UX = about[0], UY = about[1], UZ = about[2];
    
    result[0] = vec[0] * (1.0 + one_minus_cos * (UX*UX - 1.0)) +
                vec[1] * (-UZ * sin_theta + one_minus_cos * UX * UY) +
                vec[2] * (UY * sin_theta + one_minus_cos * UX * UZ);

    result[1] = vec[0] * (UZ * sin_theta + one_minus_cos * UX * UY) +
                vec[1] * (1.0 + one_minus_cos * (UY*UY - 1.0)) +
                vec[2] * (-UX * sin_theta + one_minus_cos * UY * UZ);

    result[2] = vec[0] * (-UY * sin_theta + one_minus_cos * UX * UZ) +
                vec[1] * (UX * sin_theta + one_minus_cos * UY * UZ) + 
                vec[2] * (1.0 + one_minus_cos * (UZ*UZ - 1.0));
}
