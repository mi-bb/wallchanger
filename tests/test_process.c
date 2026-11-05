/**
 * @file  test_process.c
 * @copyright Copyright (C) 2026 Michał Bąbik
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
 * @brief  Unit tests for process search functions (process/ directory).
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#if defined(__linux__)
#include <sys/prctl.h>
#endif
#include <check.h>
#include "process/process.h"
/*----------------------------------------------------------------------------*/
/* Longest process name the system keeps, longer names are cut to it */
#if defined(__FreeBSD__)
#define NAME_MAX_LEN 19
#else
#define NAME_MAX_LEN 15
#endif
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if list has an item with the given PID.
 */
static bool
list_has_pid (const ProcList *list,
              pid_t           pid)
{
    for (uint_fast32_t i = 0; i < proc_list_get_count (list); ++i) {
        if (proc_item_get_pid (proc_list_get_item (list, i)) == pid)
            return true;
    }
    return false;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create list with one process of the given name.
 */
static ProcList *
list_with_name (const char *name)
{
    ProcList *list = proc_list_new ();

    proc_list_insert (list, proc_item_new (1234, name));
    return list;
}
/*----------------------------------------------------------------------------*/
/* Name matching                                                              */
/*----------------------------------------------------------------------------*/
START_TEST (test_match_exact)
{
    ProcList *list = list_with_name ("openbox");

    ck_assert (process_list_contains (list, "openbox"));
    ck_assert (!process_list_contains (list, "openbo"));
    ck_assert (!process_list_contains (list, "openbox2"));
    ck_assert (!process_list_contains (list, ""));

    proc_list_free (list);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_match_cut_name)
{
    char      full [] = "abcdefghijklmnopqrstuvwxyz";
    char      cut  [NAME_MAX_LEN + 1];
    ProcList *list = nullptr;

    memcpy (cut, full, NAME_MAX_LEN);
    cut[NAME_MAX_LEN] = '\0';
    list = list_with_name (cut);

    /* Cut name matches the full one and itself */
    ck_assert (process_list_contains (list, full));
    ck_assert (process_list_contains (list, cut));

    /* Shorter name or different ending does not match */
    cut[NAME_MAX_LEN - 1] = '\0';
    ck_assert (!process_list_contains (list, cut));
    full[NAME_MAX_LEN - 1] = '_';
    ck_assert (!process_list_contains (list, full));

    proc_list_free (list);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_match_short_name_is_not_prefix)
{
    /* Name shorter than the limit was not cut, so prefix must not match */
    ProcList *list = list_with_name ("cinnamon");

    ck_assert (!process_list_contains (list, "cinnamon-session"));

    proc_list_free (list);
}
END_TEST
/*----------------------------------------------------------------------------*/
/* ProcList                                                                   */
/*----------------------------------------------------------------------------*/
START_TEST (test_list_grows)
{
    ProcList *list = proc_list_new ();

    ck_assert_uint_eq (proc_list_get_count (list), 0);
    ck_assert_ptr_null (proc_list_get_item (list, 0));

    for (int i = 0; i < 100; ++i)
        proc_list_insert (list, proc_item_new ((pid_t) i + 1, "proc"));

    ck_assert_uint_eq (proc_list_get_count (list), 100);
    for (int i = 0; i < 100; ++i) {
        ck_assert_int_eq (proc_item_get_pid (proc_list_get_item (list,
                        (uint_fast32_t) i)), i + 1);
    }
    ck_assert_str_eq (proc_item_get_name (proc_list_get_item (list, 99)),
                      "proc");
    ck_assert_ptr_null (proc_list_get_item (list, 100));

    proc_list_free (list);
}
END_TEST
/*----------------------------------------------------------------------------*/
/* Process scanning                                                           */
/*----------------------------------------------------------------------------*/
START_TEST (test_scan_finds_self)
{
    ProcList       *all  = process_get (nullptr, 0);
    ProcList       *mine = nullptr;
    const ProcItem *self = nullptr;

    ck_assert (list_has_pid (all, getpid ()));

    for (uint_fast32_t i = 0; i < proc_list_get_count (all); ++i) {
        if (proc_item_get_pid (proc_list_get_item (all, i)) == getpid ())
            self = proc_list_get_item (all, i);
    }
    ck_assert_ptr_nonnull (self);

    /* Search by own name finds this process */
    mine = process_get (proc_item_get_name (self), 0);
    ck_assert (list_has_pid (mine, getpid ()));
    ck_assert (process_list_contains (mine, proc_item_get_name (self)));

    proc_list_free (mine);
    proc_list_free (all);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_scan_excludes_pid)
{
    ProcList *list = process_get (nullptr, getpid ());

    ck_assert (!list_has_pid (list, getpid ()));

    proc_list_free (list);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_scan_skips_zombie)
{
    ProcList  *list = nullptr;
    siginfo_t  info;
    pid_t      child;

    child = fork ();
    ck_assert_int_ge (child, 0);
    if (child == 0)
        _exit (0);

    /* Wait for the child to exit but leave it unreaped, as a zombie */
    memset (&info, 0, sizeof (info));
    ck_assert_int_eq (waitid (P_PID, (id_t) child, &info,
                              WEXITED | WNOWAIT), 0);

    list = process_get (nullptr, 0);
    ck_assert (!list_has_pid (list, child));
    ck_assert (list_has_pid (list, getpid ()));
    proc_list_free (list);

    waitpid (child, nullptr, 0);
}
END_TEST
/*----------------------------------------------------------------------------*/
/* Killing                                                                    */
/*----------------------------------------------------------------------------*/
START_TEST (test_kill_null_name_kills_nothing)
{
    ck_assert_int_eq (process_kill_all_except_current (nullptr), 0);
}
END_TEST
/*----------------------------------------------------------------------------*/
#if defined(__linux__)
START_TEST (test_kill_by_long_name)
{
    /* Longer than 15 characters, the kernel keeps only the start */
    const char *name = "wchanger-test-victim";
    int         ready [2];
    int         status = 0;
    char        byte   = 0;
    pid_t       child;

    ck_assert_int_eq (pipe (ready), 0);

    child = fork ();
    ck_assert_int_ge (child, 0);
    if (child == 0) {
        /* check's own SIGTERM handler would kill the whole process group */
        signal (SIGTERM, SIG_DFL);
        prctl (PR_SET_NAME, name, 0, 0, 0);
        close (ready[0]);
        if (write (ready[1], "x", 1) != 1)
            _exit (1);
        for (;;)
            pause ();
    }
    close (ready[1]);
    ck_assert_int_eq (read (ready[0], &byte, 1), 1);
    close (ready[0]);

    ck_assert_int_eq (process_count_except_current (name), 1);
    ck_assert_int_eq (process_kill_all_except_current (name), 1);

    ck_assert_int_eq (waitpid (child, &status, 0), child);
    ck_assert (WIFSIGNALED (status));
    ck_assert_int_eq (WTERMSIG (status), SIGTERM);

    ck_assert_int_eq (process_count_except_current (name), 0);
}
END_TEST
#endif
/*----------------------------------------------------------------------------*/
static Suite *
process_suite (void)
{
    Suite *s        = suite_create ("Process");
    TCase *tc_match = tcase_create ("Matching");
    TCase *tc_list  = tcase_create ("List");
    TCase *tc_scan  = tcase_create ("Scan");
    TCase *tc_kill  = tcase_create ("Kill");

    tcase_add_test (tc_match, test_match_exact);
    tcase_add_test (tc_match, test_match_cut_name);
    tcase_add_test (tc_match, test_match_short_name_is_not_prefix);
    suite_add_tcase (s, tc_match);

    tcase_add_test (tc_list, test_list_grows);
    suite_add_tcase (s, tc_list);

    tcase_add_test (tc_scan, test_scan_finds_self);
    tcase_add_test (tc_scan, test_scan_excludes_pid);
    tcase_add_test (tc_scan, test_scan_skips_zombie);
    suite_add_tcase (s, tc_scan);

    tcase_add_test (tc_kill, test_kill_null_name_kills_nothing);
#if defined(__linux__)
    tcase_add_test (tc_kill, test_kill_by_long_name);
#endif
    suite_add_tcase (s, tc_kill);

    return s;
}
/*----------------------------------------------------------------------------*/
int
main (void)
{
    int      failed;
    Suite   *s  = process_suite ();
    SRunner *sr = srunner_create (s);

    srunner_run_all (sr, CK_NORMAL);
    failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
/*----------------------------------------------------------------------------*/
