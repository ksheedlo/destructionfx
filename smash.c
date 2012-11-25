/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: smash.c
 * Author: Ken Sheedlo
 *
 * Main program for smashy destruction.
 *
 ******************************************************************************/

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

#include "CSCIx229.h"

#include "kmcam.h"
#include "octree.h"
#include "dfxcube.h"
#include "util.h"

static km_camera camera;
static double aspect_ratio;
static dfx_cube cube;

static int ambient = 30;
static int diffuse = 100;
static int specular = 0;

static unsigned int texture[4];

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
        
        /* Initialize lighting */
        float ambient_vec[] = {
            0.01 * ambient,
            0.01 * ambient,
            0.01 * ambient,
            1.0
        };
        float diffuse_vec[] = {
            0.01 * diffuse,
            0.01 * diffuse,
            0.01 * diffuse,
            1.0
        };
        float specular_vec[] = {
            0.01 * specular,
            0.01 * specular,
            0.01 * specular,
            1.0
        };
        float position_vec[] = { -20.0, 0.0, 0.0, 1.0 };
        glEnable(GL_NORMALIZE);
        glEnable(GL_LIGHTING);
        //TODO: Set local viewer? Or figure out what it does
        // glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHT0);
        // Configure light 0
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_vec);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_vec);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular_vec);
        // Rotate the sun in the ecliptic plane (Boulder is 40 deg N)
        // sin(40) = 0.643, cos(40) = 0.766
        glPushMatrix();
            glRotatef(-90.0, 0.0, 0.643, 0.766);
            glLightfv(GL_LIGHT0, GL_POSITION, position_vec);
        glPopMatrix();

        /* Draw the ground */
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[0]);

        glColor3f(0.216, 0.753, 0.318);
        glBegin(GL_QUADS);
            glNormal3f(0, 1, 0);
            glTexCoord2f(0, 0);     glVertex3f(-20.0, 0.0, -20.0);
            glTexCoord2f(0, 0.475); glVertex3f(-20.0, 0.0, -1.0);
            glTexCoord2f(1, 0.475); glVertex3f( 20.0, 0.0, -1.0);
            glTexCoord2f(1, 0);     glVertex3f( 20.0, 0.0, -20.0);

            glTexCoord2f(0, 0.525); glVertex3f(-20.0, 0.0, 1.0);
            glTexCoord2f(0, 1);     glVertex3f(-20.0, 0.0, 20.0);
            glTexCoord2f(1, 1);     glVertex3f( 20.0, 0.0, 20.0);
            glTexCoord2f(1, 0.525); glVertex3f( 20.0, 0.0, 1.0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPushMatrix();
            glColor3f(0.667, 0.667, 0.667);
            glBegin(GL_QUADS);
                  glVertex3f(-20.0, 0.0, 1.0);
                  glVertex3f(-20.0, 0.0, -1.0);
                  glVertex3f( 20.0, 0.0, -1.0);
                  glVertex3f( 20.0, 0.0, 1.0);
            glEnd();
        glPopMatrix();

        dfx_cube_draw(&cube);
    
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

    dfx_cube_init(&cube);
    GLdouble blue[3] = {
        0.0, 
        0.0, 
        1.0
    };
    dfx_cube_set_pos(&cube, blue);
    dfx_cube_set_color3d(&cube, blue);
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

    texture[0] = LoadTexBMP("bitmaps/grass.bmp");
    
    check_fail_gl_error();
    glutMainLoop();
    return 0;
}
