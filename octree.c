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

void octree_init(octree_n *tree, 
                 double x, 
                 double y, 
                 double z,
                 double width, 
                 double height, 
                 double depth,
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

void octree_destroy(octree_n *tree, int32_t flags) {
    if (flags & OCTREE_FREE_DATA) {
        /* Free data members, then clear the flag */
        _octree_traverse_and_free(tree);
        flags ^= OCTREE_FREE_DATA;
    }
    gdsl_list_free(tree->volumes);
    tree->size = 0;
    for (int i = 0; i < 8; i++) {
        if (tree->child[i] != NULL) {
            octree_destroy(tree->child[i], flags);
            free(tree->child[i]);
            tree->child[i] = NULL;
        }
    }
}

int octree_insert(octree_n *tree, const octree_vol *volume, int32_t flags){
    if (!(flags & OCTREE_ALLOW_COLLISIONS)) {
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
    gdsl_element_t elt;
    if (volumes_len < OCTREE_MAX_PREFERRED_SIZE) {
        /* Insert the volume and we're done */
        if ((elt = gdsl_list_insert_head(volumes, (void *)volume)) == NULL) {
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
        if ((elt = gdsl_list_insert_head(volumes, (void *)volume)) == NULL) {
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
                int result = _octree_safe_insert(tree->child[i], vol_to_kick);
                switch (result) {
                    case TRUE:
                        gdsl_list_cursor_remove(cursor);
                        gdsl_list_cursor_free(cursor);
                        return TRUE;
                    case FALSE: break;
                    case ERROR:
                    default:
                        gdsl_list_cursor_free(cursor);
                        return result;
                }
            }
            gdsl_list_cursor_step_forward(cursor);
        }
        gdsl_list_cursor_free(cursor);
    }
    return TRUE;
}

int octree_delete(octree_vol *result, octree_n *tree, const point3d *point){
    bounding_box bounds;
    gdsl_list_t volumes = tree->volumes;
    _get_octree_bounds(&bounds, tree);
    if (!bounds_contain_point(&bounds, point)) {
        return FALSE;
    }
    gdsl_list_cursor_t cursor = gdsl_list_cursor_alloc(volumes);
    gdsl_list_cursor_move_to_head(cursor);
    size_t volumes_len = gdsl_list_get_size(volumes);
    for (uint32_t i = 0; i < volumes_len; i++) {
        octree_vol *vol = (octree_vol *)gdsl_list_cursor_get_content(cursor);
        _get_octree_volume_bounds(&bounds, vol);
        if (bounds_contain_point(&bounds, point)) {
            memcpy(result, vol, sizeof(octree_vol));
            gdsl_list_cursor_remove(cursor);
            gdsl_list_cursor_free(cursor);
            return TRUE;
        }
        gdsl_list_cursor_step_forward(cursor);
    }
    gdsl_list_cursor_free(cursor);
    if (tree->child[0] == NULL) {
        return FALSE;
    }
    for (int i = 0; i < 8; i++) {
        if (octree_delete(result, tree->child[i], point)) {
            return TRUE;
        }
    }
    return FALSE;
}

int octree_collide(const octree_n *tree, const octree_vol *volume){
    gdsl_list_cursor_t cursor;
    gdsl_list_t volumes = tree->volumes;
    bounding_box vol_box, tree_box, elt_box;

    /* If the volume is outside of the tree, smax_y now. */
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
    for (int i = 0; i < 8; i++) {
        if (tree->child[i] != NULL) {
            if (octree_collide(tree->child[i], volume)) {
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
    gdsl_list_t volumes = tree->volumes;
    size_t volumes_len = gdsl_list_get_size(volumes);
    gdsl_list_cursor_t cursor = gdsl_list_cursor_alloc(volumes);
    gdsl_list_cursor_move_to_head(cursor);
    for (uint32_t i = 0; i < volumes_len; i++) {
        octree_vol *vol = (octree_vol *)gdsl_list_cursor_get_content(cursor);
        func(vol);
        gdsl_list_cursor_step_forward(cursor);
    }
    gdsl_list_cursor_free(cursor);
    /* Traverse subtrees */
    for (int i = 0; i < 8; i++) {
        if (tree->child[i] != NULL) {
            octree_traverse(tree->child[i], func);
        }
    }
}

int octree_query_range(gdsl_list_t results, 
                       const octree_n *tree, 
                       const bounding_box *box
                       ){
    bounding_box bounds;
    gdsl_list_t volumes = tree->volumes;
    size_t volumes_len = gdsl_list_get_size(volumes);
    _get_octree_bounds(&bounds, tree);
    if (!bounds_intersect(&bounds, box)) {
        return 0;
    }
    int32_t count = 0;
    gdsl_list_cursor_t cursor = gdsl_list_cursor_alloc(volumes);
    gdsl_list_cursor_move_to_head(cursor);
    for (uint32_t i = 0; i < volumes_len; i++) {
        octree_vol *vol = (octree_vol *)gdsl_list_cursor_get_content(cursor);
        _get_octree_volume_bounds(&bounds, vol);
        if (bounds_intersect(&bounds, box)) {
            /* Add the volume to the result list */   
            gdsl_list_insert_tail(results, vol);
            count++;
        }
        gdsl_list_cursor_step_forward(cursor);
    }
    gdsl_list_cursor_free(cursor);
    for (int i = 0; i < 8; i++) {
        if (tree->child[i] != NULL) {
            count += octree_query_range(results, tree->child[i], box);
        }
    }
    return count;
}

int octree_query_line(gdsl_list_t results, const octree_n *tree, const ray3d *ray) {
    bounding_box bounds;
    gdsl_list_t volumes = tree->volumes;
    size_t volumes_len;
    int32_t count = 0;
    _get_octree_bounds(&bounds, tree);
    if (!bounds_intersect_line(NULL, &bounds, ray)) {
        return 0;
    }
    volumes_len = gdsl_list_get_size(volumes);
    gdsl_list_cursor_t cursor = gdsl_list_cursor_alloc(volumes);
    gdsl_list_cursor_move_to_head(cursor);
    for (uint32_t i = 0; i < volumes_len; i++) {
        octree_vol *vol = (octree_vol *)gdsl_list_cursor_get_content(cursor);
        _get_octree_volume_bounds(&bounds, vol);
        if (bounds_intersect_line(NULL, &bounds, ray)) {
            if(gdsl_list_insert_tail(results, vol) == NULL) {
                return ERROR;
            }
            count++;
        }
        gdsl_list_cursor_step_forward(cursor);
    }
    gdsl_list_cursor_free(cursor);
    for (int i = 0; i < 8; i++) {
        if (tree->child[i] != NULL) {
            int result = octree_query_line(results, tree->child[i], ray);
            if (result == ERROR) {
                return ERROR;
            } else {
                count += result;
            }
        }
    }
    return count;
}

void _octree_traverse_and_free(const octree_n *tree) {
    octree_traverse(tree, _octree_volume_free_data);
}

void _octree_volume_free_data(const octree_vol *volume) {
    free(volume->data);
}

int bounds_intersect(const bounding_box *lhs, const bounding_box *rhs) {
    if (lhs->max_y < rhs->min_y || lhs->min_y > rhs->max_y) {
        return FALSE;
    }
    if (lhs->max_x < rhs->min_x || lhs->min_x > rhs->max_x) {
        return FALSE;
    }
    if (lhs->max_z < rhs->min_z || lhs->min_z > rhs->max_z) {
        return FALSE;
    }
    return TRUE;
}

int bounds_contain(const bounding_box *lhs, const bounding_box *rhs) {
    if (lhs->max_y < rhs->max_y || lhs->min_y > rhs->min_y) {
        return FALSE;
    }
    if (lhs->max_x < rhs->max_x || lhs->min_x > rhs->min_x) {
        return FALSE;
    }
    if (lhs->max_z < rhs->max_z || lhs->min_z > rhs->min_z) {
        return FALSE;
    }
    return TRUE;
}

int bounds_contain_point(const bounding_box *box, const point3d *point) {
    return (box->min_x <= point->x && point->x <= box->max_x) &&
        (box->min_y <= point->y && point->y <= box->max_y) &&
        (box->min_z <= point->z && point->z <= box->max_z);
}

int bounds_intersect_line(double *results, const bounding_box *box, const ray3d *ray) {
    double dt, x, y, z;
    double tmp_results[2];
    int null_results = results == NULL;
    int i = 0;
    if (ray->dx != 0.0) {
        dt = (box->min_x - ray->sx) / ray->dx;
        y = ray->sy + (dt*ray->dy);
        z = ray->sz + (dt*ray->dz);
        if ((box->min_y <= y) && (y <= box->max_y) && (box->min_z <= z) && (z <= box->max_z)) {
            tmp_results[(i%2)] = dt;
            i++;
        }
        dt = (box->max_x - ray->sx) / ray->dx;
        y = ray->sy + (dt*ray->dy);
        z = ray->sz + (dt*ray->dz);
        if ((box->min_y <= y) && (y <= box->max_y) && (box->min_z <= z) && (z <= box->max_z)) {
            tmp_results[(i%2)] = dt;
            i++;
        }
    } else if (ray->sx < box->min_x || ray->sx > box->max_x) {
        return FALSE;
    }
    
    if (ray->dy != 0.0) {
        dt = (box->min_y - ray->sy) / ray->dy;
        x = ray->sx + (dt*ray->dx);
        z = ray->sz + (dt*ray->dz);
        if ((box->min_x <= x) && (x <= box->max_x) && (box->min_z <= z) && (z <= box->max_z)) {
            tmp_results[(i%2)] = dt;
            i++;
        }
        dt = (box->max_y - ray->sy) / ray->dy;
        x = ray->sx + (dt*ray->dx);
        z = ray->sz + (dt*ray->dz);
        if ((box->min_x <= x) && (x <= box->max_x) && (box->min_z <= z) && (z <= box->max_z)) {
            tmp_results[(i%2)] = dt;
            i++;
        }
    } else if (ray->sy < box->min_y || ray->sy > box->max_y) {
        return FALSE;
    }

    if (ray->dz != 0.0) {
        dt = (box->min_z - ray->sz) / ray->dz;
        x = ray->sx + (dt*ray->dx);
        y = ray->sy + (dt*ray->dy);
        if ((box->min_x <= x) && (x <= box->max_x) && (box->min_y <= y) && (y <= box->max_y)) {
            tmp_results[(i%2)] = dt;
            i++;
        }
        dt = (box->min_z - ray->sz) / ray->dz;
        x = ray->sx + (dt*ray->dx);
        y = ray->sy + (dt*ray->dy);
        if ((box->min_x <= x) && (x <= box->max_x) && (box->min_y <= y) && (y <= box->max_y)) {
            tmp_results[(i%2)] = dt;
            i++;
        }
    } else if (ray->sz < box->min_z || ray->sz > box->max_z) {
        return FALSE;
    }

    if (i < 2) {
        return FALSE;
    }
    if (tmp_results[0] > tmp_results[1]) {
        double tmp = tmp_results[0];
        tmp_results[0] = tmp_results[1];
        tmp_results[1] = tmp;
    }
    if (!null_results) {
        memcpy(results, tmp_results, 2*sizeof(double));
    }
    return TRUE;
}

void _get_octree_volume_bounds(bounding_box *box, const octree_vol *volume) {
    volume->get_bounds(box, volume->data);
}

void _get_octree_bounds(bounding_box *box, const octree_n *tree) {
    box->min_x = tree->x;
    box->max_x = tree->x + tree->width;
    box->min_y = tree->y; 
    box->max_y = tree->y + tree->height;
    box->min_z = tree->z;
    box->max_z = tree->z + tree->depth;
}

void write_octree_vol(const gdsl_element_t elt, FILE *output, gdsl_location_t loc, void *data) {
    octree_vol *vol = (octree_vol *)elt;
    bounding_box bounds;
    _get_octree_volume_bounds(&bounds, vol);
    if (loc == GDSL_LOCATION_HEAD) {
        fprintf(output, "[");
    }
    fprintf(output, 
            "{get_bounds: %p, data: {x: [%f, %f], y: [%f, %f], z: [%f, %f]}}%s",
            vol->get_bounds,
            bounds.min_x,
            bounds.max_x,
            bounds.min_y,
            bounds.max_y,
            bounds.min_z,
            bounds.max_z,
            loc == GDSL_LOCATION_TAIL ? "]" : ", "
            );
}
