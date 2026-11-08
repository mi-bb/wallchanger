/**
 * @file  daemon.c
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
 * @brief  Daemon related functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sysexits.h>
#include "process/process.h"
#include "daemon.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Daemonize current process.
 */
void
daemon_daemonize (void)
{
    int  res = 0;
    long i     = 0;

    res = fork ();
    if (res > 0)
        exit (EXIT_SUCCESS);
    else if (res < 0)
        err (EXIT_FAILURE, nullptr);

    if (setsid () < 0)
        err (EXIT_FAILURE, nullptr);

    signal (SIGCHLD, SIG_IGN);
    signal (SIGHUP, SIG_IGN);

    res = fork ();
    if (res > 0)
        exit (EXIT_SUCCESS);
    else if (res < 0)
        err (EXIT_FAILURE, nullptr);

    umask (0);
    res = chdir ("/");
    for (i = sysconf (_SC_OPEN_MAX); i >= 0; --i) {
        close ((int) i);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @var   STOP_WAIT_STEP_MS
 * @brief Interval between checks if stopped daemon has exited, in ms
 *
 * @var   STOP_WAIT_MAX_MS
 * @brief Longest time to wait for stopped daemon to exit, in ms
 */
constexpr long STOP_WAIT_STEP_MS = 100;
constexpr long STOP_WAIT_MAX_MS  = 5000;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sleep for the given number of milliseconds.
 *
 * @param[in] ms  Time to sleep in milliseconds
 * @return    none
 */
static void
sleep_ms (long ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000000 };

    nanosleep (&ts, nullptr);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Wait until no other wchangerd process is running.
 *
 * @return true if all wchangerd processes exited, false on timeout
 */
static bool
daemon_wait_for_exit (void)
{
    for (long waited = 0; waited < STOP_WAIT_MAX_MS;
         waited += STOP_WAIT_STEP_MS) {
        if (!daemon_check_presence ())
            return true;
        sleep_ms (STOP_WAIT_STEP_MS);
    }
    return !daemon_check_presence ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if wchangerd runs in background.
 */
bool
daemon_check_presence (void)
{
    return process_count_except_current ("wchangerd") > 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if wchangerd runs in background exit if it is.
 */
void
daemon_check_exit (void)
{
    if (daemon_check_presence ()) {
        errx (EXIT_FAILURE, "wchangerd is already running !" );
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print status of wchangerd daemon.
 */
void
daemon_print_status_exit (void)
{
    if (daemon_check_presence ()) {
        puts ("wchangerd is running");
        exit (EXIT_SUCCESS);
    }
    else {
        puts ("wchangerd is stopped");
        exit (EX_UNAVAILABLE);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Start wchangerd process
 */
void
daemon_start (void)
{
    [[maybe_unused]] int result = 0; /* Result of system command */
    result = system ("wchangerd --start &");
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kill wchangerd process
 */
int
daemon_kill (void)
{
    int cnt = 0;

    printf ("Stopping wchangerd ... ");
    /* Kill warnings go to stderr, keep the output in order */
    fflush (stdout);

    if ((cnt = process_kill_all_except_current ("wchangerd")) > 0) {
        if (daemon_wait_for_exit ()) {
            printf ("Stopped\n");
        }
        else {
            printf ("Still running after %ld seconds\n",
                    STOP_WAIT_MAX_MS / 1000);
        }
    }
    else if (daemon_check_presence ()) {
        printf ("Could not stop wchangerd\n");
    }
    else {
        printf ("Could not find wchangerd\n");
    }
    return cnt;
}
/*----------------------------------------------------------------------------*/

