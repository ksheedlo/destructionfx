/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: smash.c
 * Author: Ken Sheedlo
 *
 * Main program for smashy destruction.
 *
 ******************************************************************************/

#define GL_GLEXT_PROTOTYPES

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<float.h>
#include<time.h>

#if defined(__APPLE__) || defined(MACOSX)
    #include<GLUT/glut.h>
    #include<OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include<gdsl_list.h>

#include "CSCIx229.h"

#include "kmcam.h"
#include "octree.h"
#include "dfxcube.h"
#include "dfxfragment.h"
#include "util.h"

#define EPSILON 0.005
#define INFOBUF_SIZE 64
#define STEP_SIZE 0.17321
#define GRAVITY 3.215

#define W_RPG       0
#define W_PELLETGUN 1
#define N_WPN_TYPES 2

#define MAX_FOG_DENSITY     0.40f
#define FOG_DENSITY_STEP    0.0002f

/* Number of fragments ejected from a brick smash */
#define FRAGMENT_COUNT  4

#define FRAGMENT_SPEED  2.0

static km_camera camera;
static double aspect_ratio;
static int window_width, window_height;
static double t0 = -1.0;

static int ambient = 30;
static int diffuse = 100;
static int specular = 0;
static GLfloat fog_density;

static int dfx_count;

static unsigned int texture[4];

static octree_n tree;
static gdsl_list_t fragment_list;
static int weapon = W_RPG;

const GLfloat FOG_COLOR[4] = {
    0.8f,
    0.8f,
    0.8f,
    1.0f
};

void draw_voxel_elt(const octree_vol *volume) {
    dfx_cube *cube = (dfx_cube *)(volume->data);

    dfx_cube_draw(cube);
}

char *weapon_name(int weapon) {
    switch(weapon) {
        case W_RPG:
            return "RPG";
        case W_PELLETGUN:
            return "Pellet Gun";
    }
    return "Unknown";
}

void make_frags(point3d *point) {
    GLdouble position[3] = {
        point->x,
        point->y,
        point->z
    };
    for (int i = 0; i < FRAGMENT_COUNT; i++) {
        dfx_fragment frag;
        GLdouble vec[3];
        dfx_fragment_init(&frag);
        dfx_fragment_set_pos(&frag, position);
        random_uvec(vec, 3);
        scale_vector(vec, FRAGMENT_SPEED, 3);
        dfx_fragment_set_vel(&frag, vec);
        random_uvec(vec, 3);
        dfx_fragment_set_rotv(&frag, vec);
        gdsl_list_insert_tail(fragment_list, &frag);
    }
}

void display(void) {
    char infobuf[INFOBUF_SIZE];
    GLdouble cursor[3];
    dfx_cube test_cube;
    octree_vol vol;
    size_t fragment_ct = gdsl_list_get_size(fragment_list);
    gdsl_list_cursor_t lst_cursor;

    GLdouble offset[3] = {
        0.0, 
        0.0,
        1.5
    };
    kmcam_getpos_offset(cursor, &camera, offset);
    dfx_cube_init(&test_cube, EPSILON);
    dfx_cube_set_pos(&test_cube, cursor);
    dfx_cube_init_octree_vol(&vol, &test_cube);

    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogfv(GL_FOG_COLOR, FOG_COLOR);
    glFogf(GL_FOG_DENSITY, fog_density);
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogf(GL_FOG_START, 1.001f);
    glFogf(GL_FOG_END, 16.001f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_FOG);
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

        lst_cursor = gdsl_list_cursor_alloc(fragment_list);
        gdsl_list_cursor_move_to_head(lst_cursor);
        for (uint32_t i = 0; i < fragment_ct; i++) {
            dfx_fragment *fragment = (dfx_fragment *)gdsl_list_cursor_get_content(lst_cursor);
            dfx_fragment_draw(fragment);
            gdsl_list_cursor_step_forward(lst_cursor);
        }
        gdsl_list_cursor_free(lst_cursor);

        dfx_count = 0;
        octree_traverse(&tree, draw_voxel_elt);
        glDisable(GL_LIGHTING);

        glColor3f(1.0, 1.0, 1.0);
        snprintf(infobuf, INFOBUF_SIZE, "Current weapon: %s", weapon_name(weapon));
        write_string_wi(infobuf, 10, 10, GLUT_BITMAP_HELVETICA_12);
    glPopMatrix();  /* MODELVIEW */

    glMatrixMode(GL_PROJECTION);
    glPopMatrix(); /* PROJECTION */

    check_fail_gl_error();

    glDisable(GL_DEPTH_TEST);
    glPushMatrix(); /* PROJECTION */
        gluOrtho2D(0, window_width, 0, window_height);
        glColor3f(0.75, 0.75, 0.75);
        float half_width = window_width / 2.0f;
        float half_height = window_height / 2.0f;
        glBegin(GL_LINES);
            glVertex2f(half_width, half_height + 5.0f);
            glVertex2f(half_width, half_height + 15.0f);
            glVertex2f(half_width, half_height - 5.0f);
            glVertex2f(half_width, half_height - 15.0f);
            glVertex2f(half_width - 5.0f, half_height);
            glVertex2f(half_width - 15.0f, half_height);
            glVertex2f(half_width + 5.0f, half_height);
            glVertex2f(half_width + 15.0f, half_height);
        glEnd();
    glPopMatrix();  /* PROJECTION */
    
    glFlush();
    glutSwapBuffers();
    check_fail_gl_error();
}

void keyboard(unsigned char k, int x, int y) {
    GLdouble transform[3] = {0.0, 0.0, 0.0};
    GLdouble offset[3] = {
        0.0, 
        0.0,
        1.5
    };
    GLdouble cursor[3];
    octree_vol *vol, *min_v = NULL, elt;
    double min_t;
    point3d point;
    ray3d ray;
    bounding_box bounds;
    double results[2];
    int test;
    gdsl_list_t list, sphere_query;
    gdsl_list_cursor_t list_cursor, sphere_query_cursor;
    size_t len, sphere_query_len;

    switch (k) {
        case 'W':
        case 'w':
            /* Move one step forward */
            transform[2] = STEP_SIZE;
            kmcam_translate(&camera, transform);
            break;
        case 'A':
        case 'a':
            /* Move one step to the left */
            transform[0] = -STEP_SIZE;
            kmcam_translate(&camera, transform);
            break;
        case 'S':
        case 's':
            /* Move one step back */
            transform[2] = -STEP_SIZE;
            kmcam_translate(&camera, transform);
            break;
        case 'D':
        case 'd':
            /* Move one step to the right */
            transform[0] = STEP_SIZE;
            kmcam_translate(&camera, transform);
            break;
        case 27: /* Escape */
            exit(0);
            break;
        case ' ':
            kmcam_getpos_offset(cursor, &camera, offset);
            ray.dx = camera.center[0] - camera.position[0];
            ray.dy = camera.center[1] - camera.position[1];
            ray.dz = camera.center[2] - camera.position[2];
            ray.sx = cursor[0];
            ray.sy = cursor[1];
            ray.sz = cursor[2];
            list = gdsl_list_alloc("tmp:keyboard", NULL, NULL);
            len = octree_query_line(list, &tree, &ray);
            min_t = DBL_MAX;
            list_cursor = gdsl_list_cursor_alloc(list);
            gdsl_list_cursor_move_to_head(list_cursor);
            for (uint32_t i = 0; i < len; i++) {    
                vol = (octree_vol *)gdsl_list_cursor_get_content(list_cursor);
                _get_octree_volume_bounds(&bounds, vol);
                bounds_intersect_line(results, &bounds, &ray);
                double t = DBL_MAX;
                if (results[0] >= 0 && results[0] < t) {
                    t = results[0];
                }
                if (results[1] >= 0 && results[1] < t) {
                    t = results[1];
                }
                if (t < min_t) {
                    min_t = t;
                    min_v = vol;
                }
                gdsl_list_cursor_step_forward(list_cursor);
            }
            if (min_v != NULL) {
                _get_octree_volume_bounds(&bounds, min_v);
                point.x = (bounds.min_x + bounds.max_x) / 2.0;
                point.y = (bounds.min_y + bounds.max_y) / 2.0;
                point.z = (bounds.min_z + bounds.max_z) / 2.0;
                if (weapon == W_RPG) {
                    sphere_query = gdsl_list_alloc("tmp:keyboard:sphere", NULL, NULL);
                    sphere_query_len = octree_query_sphere(sphere_query, &tree, &point, 0.8);
                    sphere_query_cursor = gdsl_list_cursor_alloc(sphere_query);
                    gdsl_list_cursor_move_to_head(sphere_query_cursor);
                    for (uint32_t i = 0; i < sphere_query_len; i++) {
                        vol = (octree_vol *)gdsl_list_cursor_get_content(sphere_query_cursor);
                        _get_octree_volume_bounds(&bounds, vol);
                        point.x = (bounds.min_x + bounds.max_x) / 2.0;
                        point.y = (bounds.min_y + bounds.max_y) / 2.0;
                        point.z = (bounds.min_z + bounds.max_z) / 2.0;
                        test = octree_delete(&elt, &tree, &point);
                        if (test) {
                            free(elt.data);
                            make_frags(&point);
                            if (fog_density < MAX_FOG_DENSITY) {
                                fog_density += FOG_DENSITY_STEP;
                            }
                        }
                        gdsl_list_cursor_step_forward(sphere_query_cursor);
                    }
                    gdsl_list_cursor_free(sphere_query_cursor);
                    gdsl_list_free(sphere_query);
                } else if (weapon == W_PELLETGUN) {
                    test = octree_delete(&elt, &tree, &point);
                    if (test) {
                        free(elt.data);
                        make_frags(&point);
                        if (fog_density < MAX_FOG_DENSITY) {
                            fog_density += FOG_DENSITY_STEP;
                        }
                    }
                }
            }
            gdsl_list_cursor_free(list_cursor);
            gdsl_list_free(list);
            break;
        case 'f':
        case 'F':
            weapon = (weapon+1) % N_WPN_TYPES;
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
    window_width = width;
    window_height = height;
    glutPostRedisplay();
}

void idle(void) {
    double dt, t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    if (t0 < 0.0) {
        t0 = t;
    }
    dt = t - t0;
    t0 = t;

    size_t fragment_ct = gdsl_list_get_size(fragment_list);
    if (fragment_ct > 0) {
        gdsl_list_cursor_t cursor = gdsl_list_cursor_alloc(fragment_list);
        gdsl_list_cursor_move_to_head(cursor);

        for(uint32_t i = 0; i < fragment_ct; i++) {
            octree_vol vol;
            dfx_fragment *fragment = (dfx_fragment *)gdsl_list_cursor_get_content(cursor);
            dfx_fragment_step(fragment, dt, -GRAVITY);
            dfx_fragment_init_octree_vol(&vol, fragment);
            if ((!octree_contains(&tree, &vol)) || octree_collide(&tree, &vol)) {
                gdsl_list_cursor_delete(cursor);
                fragment_ct--;
            } else {
                gdsl_list_cursor_step_forward(cursor);
            }
        }
        gdsl_list_cursor_free(cursor);
    }
    glutPostRedisplay();
}

void cleanup() {
    octree_destroy(&tree, OCTREE_FREE_DATA);
    gdsl_list_free(fragment_list);
}

void init(void) {
    kmcam_init(&camera); 

    window_width = glutGet(GLUT_WINDOW_WIDTH);
    window_height = glutGet(GLUT_WINDOW_HEIGHT);
    GLdouble camera_pos[3] = {
        0.1,
        1.11,
        -6.1
    };
    kmcam_translate(&camera, camera_pos);
    aspect_ratio = 1.0;

    octree_init(&tree, -50.0, -10.0, -50.0, 100.0, 100.0, 100.0, "smashx");

    GLdouble color[3] = {
        1.0, 
        1.0, 
        1.0
    };

    /* Populate the tree with cubes */
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            for (int k = 0; k < 16; k++) {
                octree_vol vol;
                dfx_cube *cube = malloc(sizeof(*cube));
                if (cube == NULL) {
                    FATAL_ERROR("malloc dfx cube");                   
                }
                dfx_cube_init(cube, 0.2);
                GLdouble pos[3] = {
                    i * 0.2,
                    j * 0.2,
                    k * 0.2
                };
                dfx_cube_set_pos(cube, pos);
                dfx_cube_set_color3d(cube, color);
                dfx_cube_set_texture(cube, texture[1]);
                dfx_cube_init_octree_vol(&vol, cube);
                int result = octree_insert(&tree, &vol, OCTREE_DEFAULTS);
                if (result == FALSE) {
                    WARN_ERROR("octree failed to insert");
                } else if (result == ERROR) {
                    FATAL_ERROR("octree");
                }
            }
        }
    }

    //glClearColor(0.239, 0.776, 0.890, 1.0);
    glClearColor(FOG_COLOR[0], FOG_COLOR[1], FOG_COLOR[2], FOG_COLOR[3]);
    fog_density = 0.0f;

    fragment_list = gdsl_list_alloc(
                            "global:fragment", 
                            dfx_fragment_copy_alloc, 
                            dfx_fragment_copy_free
                        );

    srand48(time(NULL));
    atexit(cleanup);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Smashy Breaky Smash!");

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    texture[0] = LoadTexBMP("bitmaps/grass.bmp");
    texture[1] = LoadTexBMP("bitmaps/brick_1.bmp");
    
    init();

    check_fail_gl_error();
    glutMainLoop();
    return 0;
}
