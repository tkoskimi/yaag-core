#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>

#include "../../src/data_structures/tree.h"

static int tree_setup(void **state) {
    return 0;
}

static int tree_teardown(void **state) {
    return 0;
}

static void parse_ok_path_1(void **state) {
    char *path = "a.b.c";
    char** strs = tree_split_path( path );
    assert_string_equal( "a", strs[0] );
    assert_string_equal( "b", strs[1] );
    assert_string_equal( "c", strs[2] );
    assert_null( strs[3] );
    return;
}

static void parse_ok_path_2(void **state) {
    char *path = "itisamax.b";
    char** strs = tree_split_path( path );
    assert_string_equal( "itisamax", strs[0] );
    assert_string_equal( "b", strs[1] );
    return;
}

static void parse_nok_path_1(void **state) {
    char *path = "a.";
    char** strs = tree_split_path( path );
    assert_null( strs );
    return;
}

static void parse_nok_path_2(void **state) {
    char *path = ".a";
    char** strs = tree_split_path( path );
    assert_null( strs );
    return;
}

static void parse_nok_path_3(void **state) {
    char *path = "thisistoo.b";
    char** strs = tree_split_path( path );
    assert_null( strs );
    return;
}

static void parse_deep_tree(void **state) {
    char *path = "a.b.c.d.e.f.g.h";
    char** strs = tree_split_path( path );
    assert_string_equal( "a", strs[0] );
    assert_string_equal( "b", strs[1] );
    assert_string_equal( "c", strs[2] );
    assert_string_equal( "d", strs[3] );
    assert_string_equal( "e", strs[4] );
    assert_string_equal( "f", strs[5] );
    assert_string_equal( "g", strs[6] );
    assert_string_equal( "h", strs[7] );
    return;
}

static void parse_tree_too_deep(void **state) {
    char *path = "a.b.c.d.e.f.g.h.i";
    char** strs = tree_split_path( path );
    assert_null( strs );
    return;
}


static void clr(void **state) {
}

int tree_test(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown( parse_ok_path_1, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_ok_path_2, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_nok_path_1, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_nok_path_2, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_nok_path_3, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_deep_tree, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_tree_too_deep, tree_setup, tree_teardown ),
    };

    return cmocka_run_group_tests( tests, NULL, NULL );
}

