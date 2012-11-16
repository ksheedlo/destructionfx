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

typedef struct {
    double top, bottom, left, right, front, back;
} bounding_box;

typedef struct {
    void *data;
    void (*get_bounds)(bounding_box *, const void *);
} octree_vol;

typedef struct octree_node {
    int32_t size;
    const double x, y, z, width, height, depth;
    octree_vol data[OCTREE_MAXSIZE];
    int element_at[OCTREE_MAXSIZE];
    struct octree_node *child[8];
} octree_n;

void octree_init(octree_n *tree, 
                 const double x, 
                 const double y, 
                 const double z,
                 const double width, 
                 const double height, 
                 const double depth
                );

/* Inserts a volume into the tree.
 *
 * Volume data will be copied into the tree, rather than referenced. This means
 * that it is fine to pass an address on the stack as the address of volume and
 * it will be saved correctly into the tree.
 */
int octree_insert(octree_n *tree, const octree_vol *volume);

int octree_delete(octree_n *tree, const octree_vol *volume);

int octree_collide(const octree_n *tree, const octree_vol *volume);

void octree_traverse(const octree_n *tree, void (*func)(const octree_vol *));

int bounds_intersect(const bounding_box *lhs, const bounding_box *rhs);

/* Queries the octree for results intersecting the given volume.
 *
 * Parameters:
 *      list    A GDSL list to use for storing results. Results are returned as
 *              octree_vol types.
 *      tree    The tree to query.
 *      box     The bounds to query over.
 *
 * Returns: the number of matching entries.
 */
int octree_query_range(gdsl_list_t results, 
                       const octree_n *tree, 
                       const bounding_box *box
                       );

int _get_octree_volume_bounds(bounding_box *box, const octree_vol *volume);
