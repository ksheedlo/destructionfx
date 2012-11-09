/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: octree.h
 * Author: Ken Sheedlo
 *
 * ****************************************************************************/

#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<gdsl.h>

#define TRUE    1
#define FALSE   0

#define OCTREE_EMPTY    0
#define OCTREE_MAXSIZE  4

/* Symbolic constants for octree quadrants.
 *
 * Define north as the +z direction, east as +x, and "top" as +y. This is most
 * consistent with OpenGL.
 * Semantically, we'll label y's first, then z's, then x's. This seems most 
 * consistent with the English language. "top northeast", "bottom southwest", 
 * etc.
 */
#define BNE 0
#define BNW 1
#define BSE 2
#define BSW 3
#define TNE 4
#define TNW 5
#define TSE 6
#define TSW 7

typedef struct {
    double x, y, z;
} point3d;

typedef struct octree_node {
    int32_t size;
    double x, y, z, width, height, depth;
    point3d points[OCTREE_MAXSIZE];
    int point_at[OCTREE_MAXSIZE];
    struct octree_node *child[8];
} octree_n;

void octree_init(octree_n *tree, double x, double y, double z,
                 double width, double height, double depth);

int octree_insert(octree_n *tree, point3d *point);

void octree_delete(octree_n *tree, point3d *point, point3d *result);

int octree_collide(octree_n *tree, point3d *point);

int octree_contains(octree_n *tree, point3d *point);

int octree_intersects_volume(octree_n *tree, double x, double y, double z,
                              double width, double height, double depth);


