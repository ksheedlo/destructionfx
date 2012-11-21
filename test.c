/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: test.h
 * Author: Ken Sheedlo
 *
 * Main unit test program.
 *
 * ****************************************************************************/

#include "test.h"

static octree_n test_tree;

int octree_suite_initialize() {
    double boxen[7][6] = {
        { 1.0, 2.0, 1.0, 2.0, 1.0, 2.0 },
        { 6.0, 7.0, 1.0, 2.0, 1.0, 2.0 },
        { 1.0, 2.0, 6.0, 7.0, 1.0, 2.0 },
        { 1.0, 2.0, 1.0, 2.0, 6.0, 7.0 },
        { 60.0, 70.0, 10.0, 60.0, 40.0, 60.0 },
        { 10.0, 20.0, 80.0, 90.0, 60.0, 70.0 },
        { 80.0, 90.0, 5.0, 45.0, 5.0, 45.0 },
    };
    octree_init(&test_tree, 0.0, 0.0, 0.0, 100.0, 100.0, 100.0, "test");
    octree_vol vol;
    vol.get_bounds = get_bounding_box_bounds;
    for (int i = 0; i < 7; i++) {
        bounding_box *box = _make_bounding_box(boxen[i][0], boxen[i][1],
                                boxen[i][2], boxen[i][3], boxen[i][4], 
                                boxen[i][5]);
        if (box == NULL) {
            return 1;
        }
        vol.data = box;
        octree_insert(&test_tree, &vol, OCTREE_DEFAULTS);
    }
    return 0;
}

int octree_suite_cleanup() {
    octree_destroy(&test_tree, OCTREE_FREE_DATA);
    return 0;
}

void get_bounding_box_bounds(bounding_box *result, const void *data) {
    /* Assume data is a bounding box and just copy it */
    memcpy(result, data, sizeof(bounding_box));
}

bounding_box *_make_bounding_box(double min_x, 
                                 double max_x, 
                                 double min_y, 
                                 double max_y, 
                                 double min_z, 
                                 double max_z
                                 ) {
    bounding_box *box = malloc(sizeof(*box));
    if (box == NULL) {
        return NULL;
    }
    box->min_x = min_x;
    box->max_x = max_x;
    box->min_y = min_y;
    box->max_y = max_y;
    box->min_z = min_z;
    box->max_z = max_z;
    return box;
}

void test_octree_insert() {
    /* Test that a box was added to the tree. */
    octree_vol vol;
    vol.get_bounds = get_bounding_box_bounds;
    bounding_box *box = _make_bounding_box(30.0, 40.0, 70.0, 80.0, 50.0, 60.0);
    if (box == NULL) {
        CU_FAIL_FATAL("malloc");
    }
    vol.data = box;
    int result = octree_insert(&test_tree, &vol, OCTREE_DEFAULTS);
    CU_ASSERT_TRUE(result);

    /* Test that a box was not added to the tree. Use default flags and make
     * the new one overlap with an existing volume.
     */
    box = _make_bounding_box(32.0, 38.0, 66.6, 84.0, 55.0, 65.0);
    if (box == NULL) {
        CU_FAIL_FATAL("malloc");
    }
    vol.data = box;
    result = octree_insert(&test_tree, &vol, OCTREE_DEFAULTS);
    CU_ASSERT_FALSE(result);

    /* Test that the box will be added to the tree if we turn on the 
     * OCTREE_ALLOW_COLLISIONS flag. */
    result = octree_insert(&test_tree, &vol, OCTREE_ALLOW_COLLISIONS);
    CU_ASSERT_TRUE(result);
}

void test_octree_delete() {
    /* The tree should not delete points outside of itself. */
    point3d point;
    int result;
    octree_vol vol;

    point.x = 50.0;
    point.y = 199.9;
    point.z = 50.0;
    result = octree_delete(&vol, &test_tree, &point);
    CU_ASSERT_FALSE(result);

    /* The tree should delete the correct volume and return it's data. */
    point.x = 1.5;
    point.y = 1.5;
    point.z = 1.5;
    result = octree_delete(&vol, &test_tree, &point);
    CU_ASSERT_TRUE(result);
    bounding_box *box = (bounding_box *)vol.data;
    CU_ASSERT_EQUAL(box->max_y, 2.0);
    free(box);

    /* The tree should not delete the volume a second time. */
    result = octree_delete(&vol, &test_tree, &point);
    CU_ASSERT_FALSE(result);

    /* Nor should it delete points where nothing was to start with. */
    point.x = 35.0;
    point.y = 1.0;
    point.z = 1.0;
    result = octree_delete(&vol, &test_tree, &point);
    CU_ASSERT_FALSE(result);
}

void test_octree_collide() {
    int result;
    octree_vol vol;
    bounding_box box;
    box.min_x = 58.0;
    box.min_y = 8.0;
    box.min_z = 38.0;
    box.max_x = 62.0;
    box.max_y = 12.0;
    box.max_z = 42.0;
    vol.get_bounds = get_bounding_box_bounds;
    vol.data = &box;

    /* The tree should detect collisions when things collide. */
    result = octree_collide(&test_tree, &vol);
    CU_ASSERT_TRUE(result);

    /* But it should not detect a collision when none are present. */
    box.min_x = 50.0;
    box.max_x = 54.0;
    result = octree_collide(&test_tree, &vol);
    CU_ASSERT_FALSE(result);
}

void test_octree_contains() {
    int result;
    octree_vol vol;
    bounding_box box;
    vol.get_bounds = get_bounding_box_bounds;
    vol.data = &box;

    box.min_x = 20.0;
    box.max_x = 40.0;
    box.min_y = 20.0;
    box.max_y = 40.0;
    box.min_z = 20.0;
    box.max_z = 40.0;

    /* A dirty octree should have things inside of it */
    result = octree_contains(&test_tree, &vol);
    CU_ASSERT_TRUE(result);

    /* But it should also have standards */
    box.min_x = -30.0;
    box.max_x = -10.0;
    result = octree_contains(&test_tree, &vol);
    CU_ASSERT_FALSE(result);

    /* In particular, overlapping is not considered to be inside.
     * A compromise had to be made to make the insertion algorithm work. 
     */
    box.min_x = -10.0;
    box.max_x = 10.0;
    result = octree_contains(&test_tree, &vol);
    CU_ASSERT_FALSE(result);
}

void test_bounds_intersect() {
    bounding_box box1, box2;
    int result;

    box1.min_x = 10.0;
    box1.max_x = 20.0;
    box1.min_y = 10.0;
    box1.max_y = 20.0;
    box1.min_z = 10.0;
    box1.max_z = 20.0;

    box2.min_x = 15.0;
    box2.max_x = 25.0;
    box2.min_y = 15.0;
    box2.max_y = 25.0;
    box2.min_z = 15.0;
    box2.max_z = 25.0;

    /* It should detect when two bounding boxes intersect. */
    result = bounds_intersect(&box1, &box2);
    CU_ASSERT_TRUE(result);

    /* It should not detect an intersection when they don't. */
    box2.min_x = 35.0;
    box2.max_x = 45.0;
    box2.min_y = 35.0;
    box2.max_y = 45.0;
    box2.min_z = 35.0;
    box2.max_z = 45.0;

    result = bounds_intersect(&box1, &box2);
    CU_ASSERT_FALSE(result);
}

void test_bounds_contain() {
    bounding_box box1, box2;
    int result;

    box1.min_x = 10.0;
    box1.max_x = 20.0;
    box1.min_y = 10.0;
    box1.max_y = 20.0;
    box1.min_z = 10.0;
    box1.max_z = 20.0;

    box2.min_x = 12.0;
    box2.max_x = 15.0;
    box2.min_y = 12.0;
    box2.max_y = 15.0;
    box2.min_z = 12.0;
    box2.max_z = 15.0;

    /* It should detect when one bound contains another */
    result = bounds_contain(&box1, &box2);
    CU_ASSERT_TRUE(result);

    /* The contained bound should not contain the container */
    result = bounds_contain(&box2, &box1);
    CU_ASSERT_FALSE(result);

    /* Separate bounds are not contained */
    box2.min_x = 30.0;
    box2.max_x = 40.0;
    box2.min_y = 10.0;
    box2.max_y = 20.0;
    box2.min_z = 10.0;
    box2.max_z = 20.0;

    result = bounds_contain(&box1, &box2);
    CU_ASSERT_FALSE(result);

    /* Neither are overlapping bounds */
    box2.min_x = 15.0;
    box2.max_x = 25.0;
    box2.min_y = 12.0;
    box2.max_y = 18.0;
    box2.min_z = 12.0;
    box2.max_z = 18.0;

    result = bounds_contain(&box1, &box2);
    CU_ASSERT_FALSE(result);
}

void test_bounds_contain_point() {
    bounding_box box;
    point3d point;
    int result;

    box.min_x = 10.0;
    box.max_x = 20.0;
    box.min_y = 10.0;
    box.max_y = 20.0;
    box.min_z = 10.0;
    box.max_z = 20.0;

    point.x = 12.0;
    point.y = 12.0;
    point.z = 12.0;

    /* It should detect the point contained in bounds */
    result = bounds_contain_point(&box, &point);
    CU_ASSERT_TRUE(result);

    /* It should not contain a point outside of the box */
    point.x = 99.0;
    result = bounds_contain_point(&box, &point);
    CU_ASSERT_FALSE(result);
}

void test_octree_query_range() {
    /* XXX This would be another nice thing to test.
     *      Consider factoring into it's own test suite as other
     *      octree test functions modify the tree.
     */
}

int main() {
    CU_TestInfo bounds_tests[] = {
        { "bounds_intersect", test_bounds_intersect },
        { "bounds_contain", test_bounds_contain },
        { "bounds_contain_point", test_bounds_contain_point },
        CU_TEST_INFO_NULL,
    };
    CU_TestInfo octree_tests[] = {
        { "octree_insert", test_octree_insert },
        { "octree_delete", test_octree_delete },
        { "octree_collide", test_octree_collide },
        { "octree_contains", test_octree_contains },
        { "octree_query_range", test_octree_query_range },
        CU_TEST_INFO_NULL,
    };
    CU_SuiteInfo suites[] = {
        { "test_bounds", NULL, NULL, bounds_tests },
        {
            "test_octree", 
            octree_suite_initialize, 
            octree_suite_cleanup, 
            octree_tests 
        },
        CU_SUITE_INFO_NULL,
    };
    CU_ErrorCode error;
    if (CU_initialize_registry() != CUE_SUCCESS) {
        fprintf(stderr, "%s\n", "Failed to initialize CUnit registry");
        return 2;
    }
    if ((error = CU_register_suites(suites)) != CUE_SUCCESS) {
        fprintf(stderr, "%s:%d CUnit error: %s\n", __FILE__, __LINE__, 
            CU_get_error_msg());
        CU_cleanup_registry();
        return 4;
    }
    CU_basic_set_mode(CU_BRM_VERBOSE);
    if ((error = CU_basic_run_tests()) != CUE_SUCCESS) {
        fprintf(stderr, "%s:%d CUnit error: %s\n", __FILE__, __LINE__, 
            CU_get_error_msg());
        if (CU_get_error_action() == CUEA_ABORT) {
            CU_cleanup_registry();
            return 8;
        }
    }
    CU_cleanup_registry();
    return 0;
}
