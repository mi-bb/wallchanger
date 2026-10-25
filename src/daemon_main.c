/**
 * @file  daemon_main.c
 * @copyright Copyright (C) 2019-2026 Michał Bąbik
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @brief Wallpaper change daemon
 *
 * Automatic wallpaper changer
 *
 * @version 1.7.0
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <err.h>
#include "cmd_functions.h"
#include "daemon.h"
#include "wallpaper_change.h"
#include "randomm.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief Main function.
 *
 * @param[in] argc Arguments passed to the program from the environment in
 *                 which the program is run
 * @param[in] argv Pointer to the first element of an array of pointers that
 *                 represent the arguments passed to the program
 * @return    Return value
 */
int
main (int    argc,
      char **argv)
{
    uint32_t  time_align_count    = 0;  /* Time align parts counter */
    uint32_t  sleep_time          = 0;  /* Sleep time */
    uint32_t  change_interval     = 0;  /* Change interval value */
    uint32_t  new_change_interval = 0;  /* New change interval value */
    int       options             = 0;  /* Command line options */
    int       time_align_option   = 0;  /* Time align option */
    char     *config_file         = nullptr; /* Config file path */
    RandMem  *rand_mem            = nullptr; /* Random memory */

    /* Parse command line options */
    cmd_parse (argc, argv, &options, &config_file);

    /* Printing status */
    if (options & CMD_OPT_STATUS) {
        daemon_print_status_exit ();
    }
    /* Stopping daemon */
    if (options & CMD_OPT_STOP) {
        daemon_kill ();
        exit (EXIT_SUCCESS);
    }
    /* Restarting daemon - stopping existing one */
    if (options & CMD_OPT_RESTART && daemon_kill ()) {
        sleep500 ();
    }
    /* Check wchangerd process presence, exit if it is running */
    daemon_check_exit ();
    /* Checking if display is available */
    check_display_exit ();
    /* Check config file correctness */
    check_config_file (&config_file);
    /* Init random number structure */
    rand_mem = randomm_new ();
    /* Load settings and set wallpaper */
    change_interval = chk_setts_ch_wall (config_file, rand_mem, &time_align_option);
    /* Exiting after first wallpaper change, --once option */
    if (options & CMD_OPT_ONCE) {
        randomm_free (rand_mem);
        free (config_file);
        exit (EXIT_SUCCESS);
    }
    /* Starting daemon */
    if ((options & CMD_OPT_START) || (options & CMD_OPT_RESTART)) {
        puts ("Starting wchangerd daemon");
        daemon_daemonize ();
    }
    while (1) {
        sleep_time = change_interval;
        /* Time align enabled
         * If counter is 0 count align sleep time and
         * number of standard interval sleeps before getting
         * to full hour */
        if (time_align_option && time_align_count-- == 0) {
            sleep_time = check_time_align_val (change_interval, &time_align_count);
        }
        if (sleep_time) {
            sleep (sleep_time);
            new_change_interval = chk_setts_ch_wall (config_file, rand_mem, &time_align_option);
        }
        if (new_change_interval != change_interval) {
            change_interval = new_change_interval;
            time_align_count = 0;
        }
    }
    randomm_free (rand_mem);
    free (config_file);
    exit (EXIT_SUCCESS);
}
/*----------------------------------------------------------------------------*/

