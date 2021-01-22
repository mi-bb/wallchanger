/**
 * @file  dmfn.c
 * @copyright Copyright (C) 2019-2021 Michał Bąbik
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
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <unistd.h>
#include <signal.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sysexits.h>
#include "procfn.h"
#include "dmfn.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Daemonize current process.
 */
void
dmfn_daemonize (void)
{
    int  i_res = 0;
    long i     = 0;

    i_res = fork ();
    if (i_res > 0)
        exit (EXIT_SUCCESS);
    else if (i_res < 0)
        err (EXIT_FAILURE, NULL);

    if (setsid () < 0)
        err (EXIT_FAILURE, NULL);

    signal (SIGCHLD, SIG_IGN);
    signal (SIGHUP, SIG_IGN);

    i_res = fork ();
    if (i_res > 0)
        exit (EXIT_SUCCESS);
    else if (i_res < 0)
        err (EXIT_FAILURE, NULL);

    umask (0);
    i_res = chdir ("/");
    for (i = sysconf (_SC_OPEN_MAX); i >= 0; --i) {
        close ((int) i);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if wchangerd runs in background.
 */
int
dmfn_check_presence (void)
{
    return process_count_except_current ("wchangerd");
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if wchangerd runs in background exit if it is.
 */
void
dmfn_check_exit (void)
{
    if (dmfn_check_presence () > 0) {
        errx (EXIT_FAILURE, "wchangerd is already running !" );
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print status of wchangerd daemon.
 */
void
dmfn_print_status_exit (void)
{
    int i_cnt = 0;

    if ((i_cnt = dmfn_check_presence ()) > 0) {
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
dmfn_start (void)
{
    int i_res __attribute__ ((unused)) = 0; /* Result of system command */
    i_res = system ("wchangerd --start &");
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kill wchangerd process
 */
int
dmfn_kill (void)
{
    int i_cnt = 0;

    printf ("Stopping wchangerd ... ");

    if ((i_cnt = process_kill_all_except_current ("wchangerd")) > 0) {
        printf ("Stopped\n");
    }
    else {
        printf ("Could not find wchangerd\n");
    }
    return i_cnt;
}
/*----------------------------------------------------------------------------*/

