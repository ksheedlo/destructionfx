/*******************************************************************************
 *
 * destructionfx - CSCI 4229 Final Project Fall 2012
 * File: test.h
 * Author: Ken Sheedlo
 *
 * Unit test headers, for some of the more complicated things.
 *
 * ****************************************************************************/

#ifndef DFX_TEST_H
#define DFX_TEST_H

#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<CUnit/CUnit.h>
#include<CUnit/Basic.h>
#include<CUnit/Console.h>

#include "octree.h"

int octree_suite_initialize();

int octree_suite_cleanup();

void get_bounding_box_bounds(bounding_box *result, const void *data);

bounding_box *_make_bounding_box(double min_x, 
                                 double max_x, 
                                 double min_y, 
                                 double max_y, 
                                 double min_z, 
                                 double max_z
                                 );

void test_octree_insert();

void test_octree_delete();

void test_octree_collide();

void test_octree_contains();

void test_octree_traverse();

void test_bounds_intersect();

void test_bounds_contain();

void test_bounds_contain_point();

void test_octree_query_range();

#endif
