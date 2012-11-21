/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: smash.c
 * Author: Ken Sheedlo
 *
 * Main program for smashy destruction.
 *
 ******************************************************************************/

#include "kmcam.h"
#include "octree.h"
#include "util.h"

static km_camera camera;
static double aspect_ratio;

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); /* PROJECTION */

    gluPerspective(90.0, aspect_ratio, 1.001, 16.001);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); /* MODELVIEW */
        kmcam_place(&camera);

    
    glPopMatrix();  /* MODELVIEW */

    glMatrixMode(GL_PROJECTION);
    glPopMatrix(); /* PROJECTION */

    glFlush();
    glutSwapBuffers();
    check_fail_gl_error();
}

void keyboard(unsigned char k, int x, int y) {
    GLdouble transform[3] = {0.0, 0.0, 0.0};
    switch (k) {
        case 'W':
        case 'w':
            /* Move one step forward */
            transform[2] = 0.2;
            kmcam_translate(&camera, transform);
            break;
        case 'A':
        case 'a':
            /* Move one step to the left */
            transform[0] = -0.2;
            kmcam_translate(&camera, transform);
            break;
        case 'S':
        case 's':
            /* Move one step back */
            transform[2] = -0.2;
            kmcam_translate(&camera, transform);
            break;
        case 'D':
        case 'd':
            /* Move one step to the right */
            transform[0] = 0.2;
            kmcam_translate(&camera, transform);
            break;
        case 27: /* Escape */
            exit(0);
            break;
        default:
            return;
    }
    glutPostRedisplay();
}

void special(int k, int x, int y) {
    switch (k) {   
        case GLUT_KEY_UP:
            kmcam_rotx(&camera, 10.0);
            break;
        case GLUT_KEY_DOWN:
            kmcam_rotx(&camera, -10.0);
            break;
        case GLUT_KEY_LEFT:
            kmcam_roty(&camera, 10.0);
            break;
        case GLUT_KEY_RIGHT:
            kmcam_roty(&camera, -10.0);
            break;
        default:
            return;
    }
    glutPostRedisplay();
}

void reshape(int width, int height) {
    glViewport(0, 0, (GLint)width, (GLint)height);
    aspect_ratio = (double)width / (double)height;
    glutPostRedisplay();
}

void idle(void) {
    /* TODO: Register this with glutIdleFunc() once we figure out what the
     *       idle behavior is going to be. For now this is a no-op.
     */
}

void init(void) {
    kmcam_init(&camera); 

    GLdouble camera_pos[3] = {
        0.0,
        0.7,
        -6.0
    };
    kmcam_translate(&camera, camera_pos);
    aspect_ratio = 1.0;
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Smashy Breaky Smash!");
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    
    check_fail_gl_error();
    glutMainLoop();
    return 0;
}
