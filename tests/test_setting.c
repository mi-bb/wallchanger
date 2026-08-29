/**
 * @file  test_setting.c
 * @copyright Copyright (C) 2019-2026 Michał Bąbik
 *
 * This file is part of Wall Changer.
 *
 * Wall Changer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Wall Changer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Wall Changer.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @brief  Unit tests for the Setting structure and functions (setting.c).
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "setting.h"
#include "hashfun.h"
/*----------------------------------------------------------------------------*/
/* Creation and type accessors                                                */
/*----------------------------------------------------------------------------*/
START_TEST (test_new_int)
{
    Setting *st = setting_new_int ("myint", 42);

    ck_assert_int_eq (setting_get_type (st), SET_VAL_INT);
    ck_assert_int_eq (setting_get_int (st), 42);
    ck_assert_str_eq (setting_get_name (st), "myint");
    ck_assert_uint_eq (setting_get_hash (st), hash ("myint"));

    setting_free (st);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_new_uint)
{
    Setting *st = setting_new_uint ("myuint", 4200000000ULL);

    ck_assert_int_eq (setting_get_type (st), SET_VAL_UINT);
    ck_assert_uint_eq (setting_get_uint (st), 4200000000ULL);

    setting_free (st);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_new_double)
{
    Setting *st = setting_new_double ("mydouble", 3.5);

    ck_assert_int_eq (setting_get_type (st), SET_VAL_DOUBLE);
    ck_assert_double_eq (setting_get_double (st), 3.5);

    setting_free (st);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_new_string_copies_value)
{
    char     buf[16];
    Setting *st;

    strcpy (buf, "hello");
    st = setting_new_string ("mystr", buf);
    buf[0] = 'X'; /* Mutate original buffer, stored value must not change */

    ck_assert_int_eq (setting_get_type (st), SET_VAL_STRING);
    ck_assert_str_eq (setting_get_string (st), "hello");

    setting_free (st);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_new_string_null_value)
{
    Setting *st = setting_new_string ("mystr", nullptr);

    ck_assert_str_eq (setting_get_string (st), "");

    setting_free (st);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_new_setting_and_array_start_empty)
{
    Setting *st_set = setting_new_setting ("obj");
    Setting *st_arr = setting_new_array ("arr");

    ck_assert_int_eq (setting_get_type (st_set), SET_VAL_SETTING);
    ck_assert_ptr_null (setting_get_child (st_set));
    ck_assert_uint_eq (setting_count_children (st_set), 0);

    ck_assert_int_eq (setting_get_type (st_arr), SET_VAL_ARRAY);
    ck_assert_ptr_null (setting_get_child (st_arr));

    setting_free (st_set);
    setting_free (st_arr);
}
END_TEST
/*----------------------------------------------------------------------------*/
/* Type-mismatch and NULL safety                                              */
/*----------------------------------------------------------------------------*/
START_TEST (test_get_wrong_type_returns_default)
{
    Setting *st = setting_new_int ("i", 7);

    ck_assert_uint_eq (setting_get_uint (st), 0);
    ck_assert_double_eq (setting_get_double (st), 0);
    ck_assert_ptr_null (setting_get_string (st));

    setting_free (st);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_null_safety)
{
    ck_assert_int_eq (setting_get_type (nullptr), SET_VAL_NULL);
    ck_assert_ptr_null (setting_get_name (nullptr));
    ck_assert_uint_eq (setting_get_hash (nullptr), 0);
    ck_assert_ptr_null (setting_get_parent (nullptr));
    ck_assert_ptr_null (setting_get_child (nullptr));
    ck_assert_ptr_null (setting_next (nullptr));
    ck_assert_ptr_null (setting_prev (nullptr));
    ck_assert_int_eq (setting_get_int (nullptr), 0);
    ck_assert_ptr_null (setting_get_string (nullptr));
    ck_assert_ptr_null (settings_find (nullptr, "x"));
    ck_assert_uint_eq (settings_count (nullptr), 0);
    ck_assert_ptr_null (setting_remove (nullptr));

    /* Must not crash. */
    setting_free (nullptr);
    settings_free_all (nullptr);
}
END_TEST
/*----------------------------------------------------------------------------*/
/* String mutation                                                            */
/*----------------------------------------------------------------------------*/
START_TEST (test_set_string_overwrite)
{
    Setting *st = setting_new_string ("s", "abc");

    setting_set_string (st, "xyz");
    ck_assert_str_eq (setting_get_string (st), "xyz");

    setting_free (st);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_set_string_changes_type)
{
    Setting *st = setting_new_int ("s", 5);

    setting_set_string (st, "hi");
    ck_assert_int_eq (setting_get_type (st), SET_VAL_STRING);
    ck_assert_str_eq (setting_get_string (st), "hi");

    setting_free (st);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_set_string_null_value)
{
    Setting *st = setting_new_string ("s", "abc");

    setting_set_string (st, nullptr);
    ck_assert_str_eq (setting_get_string (st), "");

    setting_free (st);
}
END_TEST
/*----------------------------------------------------------------------------*/
/* List operations                                                            */
/*----------------------------------------------------------------------------*/
START_TEST (test_append_and_traverse)
{
    Setting *a    = setting_new_int ("a", 1);
    Setting *b    = setting_new_int ("b", 2);
    Setting *c    = setting_new_int ("c", 3);
    Setting *list = a;

    list = settings_append (list, b);
    list = settings_append (list, c);

    ck_assert_ptr_eq (list, a);
    ck_assert_ptr_eq (setting_next (a), b);
    ck_assert_ptr_eq (setting_next (b), c);
    ck_assert_ptr_null (setting_next (c));
    ck_assert_ptr_eq (setting_prev (c), b);
    ck_assert_ptr_eq (setting_prev (b), a);
    ck_assert_ptr_null (setting_prev (a));
    ck_assert_ptr_eq (setting_first (c), a);
    ck_assert_ptr_eq (setting_last (a), c);
    ck_assert_uint_eq (settings_count (list), 3);

    settings_free_all (list);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_prepend)
{
    Setting *a    = setting_new_int ("a", 1);
    Setting *b    = setting_new_int ("b", 2);
    Setting *list = a;

    list = settings_prepend (list, b);

    ck_assert_ptr_eq (list, b);
    ck_assert_ptr_eq (setting_next (b), a);
    ck_assert_ptr_eq (setting_first (a), b);

    settings_free_all (list);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_get_at_pos_and_find)
{
    Setting *a    = setting_new_int ("a", 1);
    Setting *b    = setting_new_int ("b", 2);
    Setting *c    = setting_new_int ("c", 3);
    Setting *list = a;

    list = settings_append (list, b);
    list = settings_append (list, c);

    ck_assert_ptr_eq (setting_get_at_pos (list, 0), a);
    ck_assert_ptr_eq (setting_get_at_pos (list, 1), b);
    ck_assert_ptr_eq (setting_get_at_pos (list, 2), c);
    ck_assert_ptr_null (setting_get_at_pos (list, 3));

    ck_assert_ptr_eq (settings_find (list, "b"), b);
    ck_assert_ptr_null (settings_find (list, "nope"));

    settings_free_all (list);
}
END_TEST
/*----------------------------------------------------------------------------*/
/* Child / parent operations                                                  */
/*----------------------------------------------------------------------------*/
START_TEST (test_add_child_single)
{
    Setting *parent = setting_new_setting ("parent");
    Setting *child   = setting_new_int ("c1", 1);

    setting_add_child (parent, child);

    ck_assert_ptr_eq (setting_get_child (parent), child);
    ck_assert_ptr_eq (setting_get_parent (child), parent);
    ck_assert_uint_eq (setting_count_children (parent), 1);

    settings_free_all (parent);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_add_child_multiple)
{
    Setting *parent = setting_new_setting ("parent");
    Setting *c1     = setting_new_int ("c1", 1);
    Setting *c2     = setting_new_int ("c2", 2);
    Setting *c3     = setting_new_int ("c3", 3);

    setting_add_child (parent, c1);
    setting_add_child (parent, c2);
    setting_add_child (parent, c3);

    ck_assert_ptr_eq (setting_get_child (parent), c1);
    ck_assert_uint_eq (setting_count_children (parent), 3);
    ck_assert_ptr_eq (setting_get_parent (c2), parent);
    ck_assert_ptr_eq (setting_get_parent (c3), parent);
    ck_assert_ptr_eq (setting_find_child (parent, "c2"), c2);
    ck_assert_ptr_null (setting_find_child (parent, "nope"));

    settings_free_all (parent);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_top_parent_and_top_level)
{
    Setting *root = setting_new_setting ("root");
    Setting *c1   = setting_new_int ("c1", 1);
    Setting *c2   = setting_new_int ("c2", 2);

    setting_add_child (root, c1);
    setting_add_child (root, c2);

    ck_assert_ptr_eq (setting_get_top_parent (c2), root);
    ck_assert_ptr_eq (setting_get_top_level (c2), root);

    settings_free_all (root);
}
END_TEST
/*----------------------------------------------------------------------------*/
/* Removal                                                                    */
/*----------------------------------------------------------------------------*/
START_TEST (test_remove_head)
{
    Setting *a    = setting_new_int ("a", 1);
    Setting *b    = setting_new_int ("b", 2);
    Setting *c    = setting_new_int ("c", 3);
    Setting *list = a;

    list = settings_append (list, b);
    list = settings_append (list, c);

    list = setting_remove (a);

    ck_assert_ptr_eq (list, b);
    ck_assert_ptr_null (setting_prev (b));
    ck_assert_ptr_eq (setting_next (b), c);
    ck_assert_uint_eq (settings_count (list), 2);

    settings_free_all (list);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_remove_middle)
{
    Setting *a    = setting_new_int ("a", 1);
    Setting *b    = setting_new_int ("b", 2);
    Setting *c    = setting_new_int ("c", 3);
    Setting *list = a;
    Setting *ret;

    list = settings_append (list, b);
    list = settings_append (list, c);

    ret = setting_remove (b);

    ck_assert_ptr_eq (ret, a);
    ck_assert_ptr_eq (setting_next (a), c);
    ck_assert_ptr_eq (setting_prev (c), a);
    ck_assert_uint_eq (settings_count (ret), 2);

    settings_free_all (ret);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_remove_tail)
{
    Setting *a    = setting_new_int ("a", 1);
    Setting *b    = setting_new_int ("b", 2);
    Setting *c    = setting_new_int ("c", 3);
    Setting *list = a;
    Setting *ret;

    list = settings_append (list, b);
    list = settings_append (list, c);

    ret = setting_remove (c);

    ck_assert_ptr_eq (ret, a);
    ck_assert_ptr_null (setting_next (b));
    ck_assert_uint_eq (settings_count (ret), 2);

    settings_free_all (ret);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_remove_only_item)
{
    Setting *a = setting_new_int ("a", 1);

    ck_assert_ptr_null (setting_remove (a));
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_remove_head_child_updates_parent)
{
    Setting *parent = setting_new_setting ("parent");
    Setting *c1     = setting_new_int ("c1", 1);
    Setting *c2     = setting_new_int ("c2", 2);
    Setting *c3     = setting_new_int ("c3", 3);

    setting_add_child (parent, c1);
    setting_add_child (parent, c2);
    setting_add_child (parent, c3);

    setting_remove (c1);

    ck_assert_ptr_eq (setting_get_child (parent), c2);
    ck_assert_uint_eq (setting_count_children (parent), 2);

    settings_free_all (parent);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_remove_middle_child_keeps_parent_pointer)
{
    Setting *parent = setting_new_setting ("parent");
    Setting *c1     = setting_new_int ("c1", 1);
    Setting *c2     = setting_new_int ("c2", 2);
    Setting *c3     = setting_new_int ("c3", 3);

    setting_add_child (parent, c1);
    setting_add_child (parent, c2);
    setting_add_child (parent, c3);

    setting_remove (c2);

    ck_assert_ptr_eq (setting_get_child (parent), c1);
    ck_assert_uint_eq (setting_count_children (parent), 2);

    settings_free_all (parent);
}
END_TEST
/*----------------------------------------------------------------------------*/
/* Replace / find-and-replace / append-or-* helpers                          */
/*----------------------------------------------------------------------------*/
START_TEST (test_setting_replace_middle)
{
    Setting *a    = setting_new_int ("a", 1);
    Setting *b    = setting_new_int ("b", 2);
    Setting *c    = setting_new_int ("c", 3);
    Setting *x    = setting_new_int ("x", 99);
    Setting *list = a;
    Setting *ret;

    list = settings_append (list, b);
    list = settings_append (list, c);

    ret = setting_replace (b, x);

    ck_assert_ptr_eq (ret, a);
    ck_assert_ptr_eq (setting_next (a), x);
    ck_assert_ptr_eq (setting_next (x), c);
    ck_assert_ptr_eq (setting_prev (x), a);
    ck_assert_ptr_eq (setting_prev (c), x);

    settings_free_all (ret);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_find_replace_stops_at_first_match)
{
    Setting *a    = setting_new_int ("dup", 1);
    Setting *b    = setting_new_int ("dup", 2);
    Setting *x    = setting_new_int ("dup", 99);
    Setting *list = a;
    Setting *ret;

    list = settings_append (list, b);

    ret = settings_find_replace (list, x);

    ck_assert_ptr_eq (ret, x);
    ck_assert_int_eq (setting_get_int (x), 99);
    /* Second "dup" (b) must be untouched by the replace. */
    ck_assert_ptr_eq (setting_next (x), b);
    ck_assert_int_eq (setting_get_int (setting_next (x)), 2);

    settings_free_all (ret);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_append_or_ignore_existing_name_is_ignored)
{
    Setting *a       = setting_new_int ("dup", 1);
    Setting *dup_new = setting_new_int ("dup", 99);

    settings_append_or_ignore (a, dup_new);

    ck_assert_uint_eq (settings_count (a), 1);
    ck_assert_int_eq (setting_get_int (a), 1);

    setting_free (dup_new);
    settings_free_all (a);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_append_or_ignore_new_name_is_appended)
{
    Setting *a   = setting_new_int ("a", 1);
    Setting *b   = setting_new_int ("b", 2);

    settings_append_or_ignore (a, b);

    ck_assert_uint_eq (settings_count (a), 2);
    ck_assert_ptr_eq (setting_next (a), b);

    settings_free_all (a);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_append_or_replace_existing_name_head)
{
    Setting *a       = setting_new_int ("dup", 1);
    Setting *dup_new = setting_new_int ("dup", 99);
    Setting *ret;

    ret = settings_append_or_replace (a, dup_new);

    ck_assert_ptr_eq (ret, dup_new);
    ck_assert_int_eq (setting_get_int (ret), 99);
    ck_assert_uint_eq (settings_count (ret), 1);

    settings_free_all (ret);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_append_or_replace_existing_name_non_head)
{
    Setting *a       = setting_new_int ("a", 1);
    Setting *dup     = setting_new_int ("dup", 2);
    Setting *dup_new = setting_new_int ("dup", 99);
    Setting *list    = settings_append (a, dup);
    Setting *ret;

    ret = settings_append_or_replace (list, dup_new);

    ck_assert_ptr_eq (ret, a);
    ck_assert_ptr_eq (setting_next (a), dup_new);
    ck_assert_int_eq (setting_get_int (setting_next (a)), 99);

    settings_free_all (ret);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_append_or_replace_new_name_is_appended)
{
    Setting *a   = setting_new_int ("a", 1);
    Setting *b   = setting_new_int ("b", 2);
    Setting *ret;

    ret = settings_append_or_replace (a, b);

    ck_assert_ptr_eq (ret, a);
    ck_assert_uint_eq (settings_count (a), 2);
    ck_assert_ptr_eq (setting_next (a), b);

    settings_free_all (ret);
}
END_TEST
/*----------------------------------------------------------------------------*/
/* Misc                                                                       */
/*----------------------------------------------------------------------------*/
START_TEST (test_settings_equal_names)
{
    Setting *a1 = setting_new_int ("same", 1);
    Setting *a2 = setting_new_string ("same", "x");
    Setting *b  = setting_new_int ("different", 1);

    ck_assert_int_eq (settings_equal_names (a1, a2), 1);
    ck_assert_int_eq (settings_equal_names (a1, b), 0);

    setting_free (a1);
    setting_free (a2);
    setting_free (b);
}
END_TEST
/*----------------------------------------------------------------------------*/
static Suite *
setting_suite (void)
{
    Suite *s   = suite_create ("Setting");
    TCase *tc_create  = tcase_create ("Creation");
    TCase *tc_access  = tcase_create ("Accessors");
    TCase *tc_mutate  = tcase_create ("StringMutation");
    TCase *tc_list    = tcase_create ("ListOps");
    TCase *tc_child   = tcase_create ("ChildOps");
    TCase *tc_remove  = tcase_create ("Removal");
    TCase *tc_replace = tcase_create ("Replace");
    TCase *tc_misc    = tcase_create ("Misc");

    tcase_add_test (tc_create, test_new_int);
    tcase_add_test (tc_create, test_new_uint);
    tcase_add_test (tc_create, test_new_double);
    tcase_add_test (tc_create, test_new_string_copies_value);
    tcase_add_test (tc_create, test_new_string_null_value);
    tcase_add_test (tc_create, test_new_setting_and_array_start_empty);
    suite_add_tcase (s, tc_create);

    tcase_add_test (tc_access, test_get_wrong_type_returns_default);
    tcase_add_test (tc_access, test_null_safety);
    suite_add_tcase (s, tc_access);

    tcase_add_test (tc_mutate, test_set_string_overwrite);
    tcase_add_test (tc_mutate, test_set_string_changes_type);
    tcase_add_test (tc_mutate, test_set_string_null_value);
    suite_add_tcase (s, tc_mutate);

    tcase_add_test (tc_list, test_append_and_traverse);
    tcase_add_test (tc_list, test_prepend);
    tcase_add_test (tc_list, test_get_at_pos_and_find);
    suite_add_tcase (s, tc_list);

    tcase_add_test (tc_child, test_add_child_single);
    tcase_add_test (tc_child, test_add_child_multiple);
    tcase_add_test (tc_child, test_top_parent_and_top_level);
    suite_add_tcase (s, tc_child);

    tcase_add_test (tc_remove, test_remove_head);
    tcase_add_test (tc_remove, test_remove_middle);
    tcase_add_test (tc_remove, test_remove_tail);
    tcase_add_test (tc_remove, test_remove_only_item);
    tcase_add_test (tc_remove, test_remove_head_child_updates_parent);
    tcase_add_test (tc_remove, test_remove_middle_child_keeps_parent_pointer);
    suite_add_tcase (s, tc_remove);

    tcase_add_test (tc_replace, test_setting_replace_middle);
    tcase_add_test (tc_replace, test_find_replace_stops_at_first_match);
    tcase_add_test (tc_replace, test_append_or_ignore_existing_name_is_ignored);
    tcase_add_test (tc_replace, test_append_or_ignore_new_name_is_appended);
    tcase_add_test (tc_replace, test_append_or_replace_existing_name_head);
    tcase_add_test (tc_replace, test_append_or_replace_existing_name_non_head);
    tcase_add_test (tc_replace, test_append_or_replace_new_name_is_appended);
    suite_add_tcase (s, tc_replace);

    tcase_add_test (tc_misc, test_settings_equal_names);
    suite_add_tcase (s, tc_misc);

    return s;
}
/*----------------------------------------------------------------------------*/
int
main (void)
{
    int      i_failed;
    Suite   *s  = setting_suite ();
    SRunner *sr = srunner_create (s);

    srunner_run_all (sr, CK_NORMAL);
    i_failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (i_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
/*----------------------------------------------------------------------------*/
