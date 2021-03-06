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
#include<math.h>

#include<gdsl_list.h>

#define TRUE    1
#define FALSE   0
#define ERROR   -1

/* Magic numbers */
#define OCTREE_MAX_PREFERRED_SIZE   8
#define _MAX_NAME_LENGTH            64

/* Flags */
#define OCTREE_ALLOW_COLLISIONS     1
#define OCTREE_FREE_DATA            2

/* Flag defaults */
#define OCTREE_DEFAULTS      0


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
    double max_y, min_y, min_x, max_x, max_z, min_z;
} bounding_box;

typedef struct {
    double sx, sy, sz;
    double dx, dy, dz;
} ray3d;

typedef struct {
    void *data;
    void (*get_bounds)(bounding_box *, const void *);
} octree_vol;

typedef struct octree_node {
    int32_t size;
    double x, y, z, width, height, depth;
    gdsl_list_t volumes;
    struct octree_node *child[8];
} octree_n;

void octree_init(octree_n *tree, 
                 double x, 
                 double y, 
                 double z,
                 double width, 
                 double height, 
                 double depth,
                 const char *name
                );

int _octree_initialize_children(octree_n *tree);

gdsl_element_t _octree_elt_alloc(void *data);

void _octree_elt_free(gdsl_element_t elt);

void octree_destroy(octree_n *tree, int32_t flags);

/* Inserts a volume into the tree.
 *
 * Volume data will be copied into the tree, rather than referenced. This means
 * that it is fine to pass an address on the stack as the address of volume and
 * it will be saved correctly into the tree.
 */
int octree_insert(octree_n *tree, const octree_vol *volume, int32_t flags);

int _octree_safe_insert(octree_n *tree, const octree_vol *volume);

int octree_delete(octree_vol *result, octree_n *tree, const point3d *point);

int octree_collide(const octree_n *tree, const octree_vol *volume);

int octree_contains(const octree_n *tree, const octree_vol *volume);

void octree_traverse(const octree_n *tree, void (*func)(const octree_vol *));

void _octree_traverse_and_free(const octree_n *tree);

void _octree_volume_free_data(const octree_vol *volume);

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

int bounds_contain_point(const bounding_box *box, const point3d *point);

/* Tests box and a ray for intersection.
 *
 * If the box and ray intersect, returns TRUE and places t-values in results[0]
 * and results[1]. Otherwise, FALSE is returned and results is not modified.
 * If results is NULL, it will not be modified regardless of whether an 
 * intersection is detected.
 */
int bounds_intersect_line(double *results, const bounding_box *box, const ray3d *ray);

int bounds_intersect_sphere(
                    const bounding_box *box, 
                    const point3d *center, 
                    const double radius
                );

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

int octree_query_line(gdsl_list_t results, const octree_n *tree, const ray3d *ray);

int octree_query_sphere(
                    gdsl_list_t results, 
                    const octree_n *tree, 
                    const point3d *center, 
                    const double radius
                );

void _get_octree_volume_bounds(bounding_box *box, const octree_vol *volume);

void _get_octree_bounds(bounding_box *box, const octree_n *tree);

double _point3d_d3_dist(
                    const point3d *point, 
                    const double x, 
                    const double y, 
                    const double z
                );

void write_octree_vol(const gdsl_element_t elt, FILE *output, gdsl_location_t loc, void *data);

#endif
