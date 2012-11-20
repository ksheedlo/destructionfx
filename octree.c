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
                 double width, double height, double depth,
                 const char *name){
    tree->size = 0;
    tree->x = x;
    tree->y = y;
    tree->z = z;
    tree->width = width;
    tree->height = height;
    tree->depth = depth;
    tree->volumes = gdsl_list_alloc(name, _octree_elt_alloc, _octree_elt_free);
    
    for (int32_t i = 0; i < 8; i++) {
        tree->child[i] = NULL;
    }
}

int _octree_initialize_children(octree_n *tree) {
    /* Initialize child subtrees */
    double half_height = tree->height / 2.0;
    double half_width = tree->width / 2.0;
    double half_depth = tree->depth / 2.0;
    
    for (int i = 0; i < 8; i++){
        tree->child[i] = malloc(sizeof(octree_n));
        if (tree->child[i] == NULL) {
            fprintf(stderr, "Memory allocation failed at %s:%d.\n", 
                __FILE__, __LINE__);
            return ERROR;
        }
        double x = tree->x, y = tree->y, z = tree->z;
        switch (i) {
            case BNE: 
                x += half_width;
                z += half_depth;
                break;
            case BNW: 
                z += half_depth;
                break;
            case BSE: 
                x += half_width;
                break;
            case BSW: 
                break;
            case TNE: 
                x += half_width;
                y += half_height;
                z += half_depth;
                break;
            case TNW: 
                y += half_height;
                z += half_depth;
                break;
            case TSE: 
                x += half_width;
                y += half_height;
                break;
            case TSW: 
                y += half_height;
                break;
        }
        char name[_MAX_NAME_LENGTH];
        snprintf(name, _MAX_NAME_LENGTH, "%s%d", 
                 gdsl_list_get_name(tree->volumes), i);

        octree_init(tree->child[i], x, y, z, 
                    half_width, 
                    half_height, 
                    half_depth,
                    name
                    );
    }
    return TRUE;
}

gdsl_element_t _octree_elt_alloc(void *data) {
    octree_vol *new_vol = malloc(sizeof(*new_vol));
    memcpy(new_vol, data, sizeof(octree_vol));
    return (gdsl_element_t)new_vol;
}

void _octree_elt_free(gdsl_element_t elt) {
    free(elt);
}

int octree_insert(octree_n *tree, const octree_vol *volume, int32_t flags){
    if (!(flags | OCTREE_ALLOW_COLLISIONS)) {
        /* Don't allow collisions. */
        if (octree_collide(tree, volume)) {
            return FALSE;
        }
    }
    return _octree_safe_insert(tree, volume);
}

int _octree_safe_insert(octree_n *tree, const octree_vol *volume) {
    gdsl_list_t volumes = tree->volumes;
    size_t volumes_len = gdsl_list_get_size(volumes);

    if (!octree_contains(tree, volume)) {
        return FALSE;
    }
    if (volumes_len < OCTREE_MAX_PREFERRED_SIZE) {
        /* Insert the volume and we're done */
        gdsl_element_t elt;
        if ((elt = gdsl_list_insert_head(volumes, volume)) == NULL) {
            fprintf(stderr, "Memory allocation failed at %s:%d.\n", 
                __FILE__, __LINE__);
            return ERROR;
        }
        return TRUE;
    } else {
        if (tree->child[0] == NULL) {
            _octree_initialize_children(tree);
        }
        for (int i = 0; i < 8; i++) {
            int result = _octree_safe_insert(tree->child[i], volume);
            if ((result == TRUE) || (result == ERROR)) {
                return result;
            }
        }
        /* All children failed to insert the volume. At this point, the volume
         * must be overlapping several of this node's suboctants. We should add
         * it to volumes and try to kick one of the present volumes down the
         * tree.
         */
        if ((elt = gdsl_list_insert_head(volumes, volume)) == NULL) {
            fprintf(stderr, "Memory allocation failed at %s:%d.\n", 
                __FILE__, __LINE__);
            return ERROR;
        }

        volumes_len = gdsl_list_get_size(volumes);
        gdsl_list_cursor_t cursor = gdsl_list_cursor_alloc(volumes);
        gdsl_list_cursor_move_to_head(cursor);
        for (uint32_t i = 0; i < volumes_len; i++) {
            octree_vol *vol_to_kick = 
                (octree_vol *)gdsl_list_cursor_get_content(cursor);
            for (int i = 0; i < 8; i++) {
                int result = _octree_safe_insert(tree->child[i], volume);
                switch (result) {
                    case TRUE:
                        gdsl_list_cursor_remove(cursor);
                        gdsl_list_cursor_free(cursor);
                        return TRUE;
                    case FALSE: break;
                    case ERROR:
                    default:
                        gdsl_list_cursor_free(cursor);
                        return result
                }
            }
            gdsl_list_cursor_step_forward(cursor);
        }
        gdsl_list_cursor_free(cursor);
    }
    return TRUE;
}

int octree_delete(octree_n *tree, const octree_vol *volume){
    
    return 0;
}

int octree_collide(const octree_n *tree, const octree_vol *volume){
    gdsl_element_t elt;
    gdsl_list_cursor_t cursor;
    gdsl_list_t volumes = tree->volumes;
    bounding_box vol_box, tree_box, elt_box;

    /* If the volume is outside of the tree, stop now. */
    _get_octree_bounds(&tree_box, tree);
    _get_octree_volume_bounds(&vol_box, volume);
    if (!bounds_intersect(&tree_box, &vol_box)) {
        return FALSE;
    }

    /* Check the local objects. */
    size_t volumes_len = gdsl_list_get_size(volumes);
    cursor = gdsl_list_cursor_alloc(volumes);
    gdsl_list_cursor_move_to_head(cursor);
    for (uint32_t i = 0; i < volumes_len; i++) {
        octree_vol *vol_elt = (octree_vol *)gdsl_list_cursor_get_content(cursor);
        _get_octree_volume_bounds(&elt_box, vol_elt);
        if (bounds_intersect(&vol_box, &elt_box)) {
            gdsl_list_cursor_free(cursor);
            return TRUE;
        }
        gdsl_list_cursor_step_forward(cursor);
    }
    gdsl_list_cursor_free(cursor);

    /* Recurse on child subtrees. */
    if (tree->child[i] != NULL) {
        for (int i = 0; i < 8; i++) {
            if (octree_collide(tree, volume)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

int octree_contains(const octree_n *tree, const octree_vol *volume) {
    bounding_box tree_box, vol_box;
    _get_octree_volume_bounds(&vol_box, volume);
    _get_octree_bounds(&tree_box, tree);

    return bounds_contain(&tree_box, &vol_box);
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
        return FALSE;
    }
    if (lhs->right < rhs->left || lhs->left > rhs->right) {
        return FALSE;
    }
    if (lhs->front < rhs->back || lhs->back > rhs->front) {
        return FALSE;
    }
    return TRUE;
}

int bounds_contain(const bounding_box *lhs, const bounding_box *rhs) {
    if (lhs->top < rhs->top || lhs->bottom > rhs->bottom) {
        return FALSE;
    }
    if (lhs->right < rhs->right || lhs->left > rhs->left) {
        return FALSE;
    }
    if (lhs->front < rhs->front || lhs->back > rhs->back) {
        return FALSE;
    }
    return TRUE;
}

void _get_octree_volume_bounds(bounding_box *box, const octree_vol *volume) {
    volume->get_bounds(box, volume->data);
}

void _get_octree_bounds(bounding_box *box, const octree_n *tree) {
    box->left = tree->x;
    box->right = tree->x + tree->width;
    box->bottom = tree->y; 
    box->top = tree->y + tree->height;
    box->back = tree-z;
    box->front = tree->z + tree->depth;
}
