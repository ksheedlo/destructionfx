/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: octree.h
 * Author: Ken Sheedlo
 *
 * Just a basic octree implementation.
 *
 * ****************************************************************************/

#include "octree.h"

void octree_init(octree_n *tree, double x, double y, double z,
                 double width, double height, double depth){
    tree->size = OCTREE_EMPTY;
    tree->x = x;
    tree->y = y;
    tree->z = z;
    tree->width = width;
    tree->height = height;
    tree->depth = depth;
    
    for (int32_t i = 0; i < OCTREE_MAXSIZE; i++) {
        tree->points[i].x = 0.0;
        tree->points[i].y = 0.0;
        tree->points[i].z = 0.0;
        tree->point_at[i] = FALSE;
    }
    for (int32_t i = 0; i < 8; i++) {
        tree->child[i] = NULL;
    }
}

int octree_insert(octree_n *tree, point3d *point){
    if (!octree_contains(tree, point)) {
        return 0;
    }
    if (tree->size < OCTREE_MAXSIZE) {
        /* Insert the point into the first available slot */
        for (int32_t i = 0; i < OCTREE_MAXSIZE; i++) {
            if (!(tree->point_at[i])) {
                memcpy((tree->points + i), point, sizeof(point3d));
                tree->point_at[i] = TRUE;
                break;
            }
        }
    } else {
        if (tree->child[0] == NULL) {
            /* Initialize child subtrees */
            for (int i = 0; i < 8; i++){
                tree->child[i] = malloc(sizeof(octree_n));
                if (tree->child[i] == NULL) {
                    fprintf(stderr, "Memory allocation failed at %s:%d.\n", 
                        __FILE__, __LINE__);
                    return 0;
                }
                double half_height = tree->height / 2.0;
                double half_width = tree->width / 2.0;
                double half_depth = tree->depth / 2.0;

                switch (i) {
                    case BNE: 
                        octree_init(tree->child[i], tree->x + half_width, 
                                    tree->y, tree->z + half_depth, half_width, 
                                    half_height, half_depth);
                        break;
                    case BNW: 
                        octree_init(tree->child[i], tree->x, 
                                    tree->y, tree->z + half_depth, half_width,
                                    half_height, half_depth);
                        break;
                    case BSE: 
                        octree_init(tree->child[i], tree->x + half_width, 
                                    tree->y, tree->z, half_width, half_height,
                                    half_depth);
                        break;
                    case BSW: 
                        octree_init(tree->child[i], tree->x, 
                                    tree->y, tree->z, half_width, half_height,
                                    half_depth);
                        break;
                    case TNE: 
                        octree_init(tree->child[i], tree->x + half_width, 
                                    tree->y + half_height, tree->z + half_depth, 
                                    half_width, half_height, half_depth);
                        break;
                    case TNW: 
                        octree_init(tree->child[i], tree->x, 
                                    tree->y + half_height, tree->z + half_depth, 
                                    half_width, half_height, half_depth);
                        break;
                    case TSE: 
                        octree_init(tree->child[i], tree->x + half_width, 
                                    tree->y + half_height, tree->z, half_width, 
                                    half_height, half_depth);
                        break;
                    case TSW: 
                        octree_init(tree->child[i], tree->x, 
                                    tree->y + half_height, tree->z, half_width, 
                                    half_height, half_depth);
                        break;
                }
            }
        }
        /* Add the new point to a child */
    }

    /* This line should never be reached */
    return 0;
}

void octree_delete(octree_n *tree, point3d *point, point3d *result){
    
}

int octree_collide(octree_n *tree, point3d *point){
    
    return 0;
}

int octree_contains(octree_n *tree, point3d *point){
    
    return 0;
}

int octree_intersects_volume(octree_n *tree, double x, double y, double z,
                              double width, double height, double depth){

    return 0;
}


