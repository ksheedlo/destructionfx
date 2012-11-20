/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: octree.h
 * Author: Ken Sheedlo
 *
 * ****************************************************************************/

#ifndef OCTREE_H
#define OCTREE_H

#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<gdsl.h>

#define TRUE    1
#define FALSE   0
#define ERROR   2

/* Magic numbers */
#define OCTREE_EMPTY                0
#define OCTREE_MAX_PREFERRED_SIZE   4
#define _MAX_NAME_LENGTH            64

/* Flags */
#define OCTREE_ALLOW_COLLISIONS     1

/* Flag defaults */
#define OCTREE_INSERT_DEFAULTS      0


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

/* Bounding Box
 *
 * Define top as +y, right as +x, and front as +z.
 */

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
    gdsl_list_t volumes;
    struct octree_node *child[8];
} octree_n;

void octree_init(octree_n *tree, 
                 const double x, 
                 const double y, 
                 const double z,
                 const double width, 
                 const double height, 
                 const double depth,
                 char *name
                );

int _octree_initialize_children(octree_n *tree);

gdsl_element_t _octree_elt_alloc(void *data);

void _octree_elt_free(gdsl_element_t elt);

/* Inserts a volume into the tree.
 *
 * Volume data will be copied into the tree, rather than referenced. This means
 * that it is fine to pass an address on the stack as the address of volume and
 * it will be saved correctly into the tree.
 */
int octree_insert(octree_n *tree, const octree_vol *volume, int32_t flags);

int _octree_safe_insert(octree_n *tree, const octree_vol *volume);

int octree_delete(octree_vol *result, octree_n *tree, const octree_vol *volume);

int octree_collide(const octree_n *tree, const octree_vol *volume);

int octree_contains(const octree_n *tree, const octree_vol *volume);

void octree_traverse(const octree_n *tree, void (*func)(const octree_vol *));

/* Checks to determine whether lhs and rhs intersect.
 *
 * Parameters:
 *      lhs     Bounding box
 *      rhs     Bounding box
 *
 * Returns: a boolean
 */
int bounds_intersect(const bounding_box *lhs, const bounding_box *rhs);

/* Checks to determine whether lhs contains rhs.
 *
 * Parameters:
 *      lhs     Bounding box
 *      rhs     Bounding box
 *
 * Returns: a boolean
 */
int bounds_contain(const bounding_box *lhs, const bounding_box *rhs);

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

void _get_octree_volume_bounds(bounding_box *box, const octree_vol *volume);

void _get_octree_bounds(bounding_box *box, const octree_n *tree);

#endif
