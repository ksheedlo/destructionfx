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
        tree->point_at[i] = FALSE;
    }
    for (int32_t i = 0; i < 8; i++) {
        tree->child[i] = NULL;
    }
}

int octree_insert(octree_n *tree, octree_vol *volume){
    if (!octree_collide(tree, volume)) {
        return 0;
    }
    /* TODO: figure out how to insert a volume */
#if 0
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

#endif
    /* This line should never be reached */
    return 0;
}

int octree_delete(octree_n *tree, const octree_vol *volume){
    
    return 0;
}

int octree_collide(const octree_n *tree, const octree_vol *volume){
    
    return 0;
}

void octree_traverse(const octree_n *tree, void (*func)(const octree_vol *)) {
    /* Call func on tree's data elements */
    for (int i = 0; i < OCTREE_MAXSIZE; i++) {
        if (tree->element_at[i]) {
            func(&(tree->data[i]));
        }
    }
    /* Traverse subtrees */
    for (int i = 0; i < 8; i++) {
        if (tree->child[i] != NULL) {
            octree_traverse(tree->child[i], func);
        }
    }
}

int bounds_intersect(const bounding_box *lhs, const bounding_box *rhs) {
    if (lhs->top < rhs->bottom || lhs->bottom > rhs->top) {
        return 0;
    }
    if (lhs->right < rhs->left || lhs->left > rhs->right) {
        return 0;
    }
    if (lhs->front < rhs->back || lhs->back > rhs->front) {
        return 0;
    }
    return 1;
}

int _get_octree_volume_bounds(bounding_box *box, const octree_vol *volume) {
    volume->get_bounds(box, volume->data);
}
