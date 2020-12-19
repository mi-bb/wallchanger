/**
 * @file  wcngdmn.c
 * @copyright Copyright (C) 2019-2020 Michał Bąbik
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
 * @brief Wallpaper changer
 *
 * Automatic wallpaper changer
 *
 * @date December 19, 2020
 *
 * @version 1.6.15
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <err.h>
#include "cmdfn.h"
#include "dmfn.h"
#include "chkwch.h"
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
    uint32_t  ui_cnt       = 0;    /* Time align parts counter */
    uint32_t  ui_sleep     = 0;    /* Sleep time */
    uint32_t  ui_ch_int    = 0;    /* Change interval value */
    uint32_t  ui_ch_int_n  = 0;    /* New change interval value */
    int       i_opt        = 0;    /* Command line options */
    int       i_atime_opt  = 0;    /* Time align option */
    char     *s_cfgfile    = NULL; /* Config file path */
    RandMem  *rm_rand      = NULL; /* Ramdom memory */

    /* Parse command line options */
    cmdfn_parse (argc, argv, &i_opt, &s_cfgfile);

    /* Printing status */
    if (i_opt & CMD_OPT_STATUS) {
        dmfn_print_status_exit ();
    }
    /* Stopping daemon */
    if (i_opt & CMD_OPT_STOP) {
        dmfn_kill ();
        exit (EXIT_SUCCESS);
    }
    /* Restarting daemon - stopping existing one */
    if (i_opt & CMD_OPT_RESTART && dmfn_kill ()) {
        sleep500 ();
    }
    /* Check wchangerd process presence, exit if it is running */
    dmfn_check_exit ();
    /* Checking if display is available */
    check_display_exit ();
    /* Check config file correctness */
    check_config_file (&s_cfgfile);
    /* Init random number structure */
    rm_rand = randomm_new ();
    /* Load settings and set wallpaper */
    ui_ch_int = chk_setts_ch_wall (s_cfgfile, rm_rand, &i_atime_opt);
    /* Exiting after first wallpaper change, --once option */
    if (i_opt & CMD_OPT_ONCE) {
        randomm_free (rm_rand);
        free (s_cfgfile);
        exit (EXIT_SUCCESS);
    }
    /* Starting daemon */
    if ((i_opt & CMD_OPT_START) || (i_opt & CMD_OPT_RESTART)) {
        puts ("Starting wchangerd daemon");
        dmfn_daemonize ();
    }
    while (1) {
        ui_sleep = ui_ch_int;
        /* Time align enabled */
        if (i_atime_opt) {
            /* If counter is 0 count align sleep time and
             * number of standard interval sleeps before getting
             * to full hour */
            if (ui_cnt > 0) {
                --ui_cnt;
            }
            else {
                ui_sleep = check_time_align_val (ui_ch_int, &ui_cnt);
            }
        }
        if (ui_sleep != 0) {
            sleep (ui_sleep);
            ui_ch_int_n = chk_setts_ch_wall (s_cfgfile, rm_rand, &i_atime_opt);
        }
        if (ui_ch_int_n != ui_ch_int) {
            ui_ch_int = ui_ch_int_n;
            ui_cnt = 0;
        }
    }
    randomm_free (rm_rand);
    free (s_cfgfile);
    exit (EXIT_SUCCESS);
}
/*----------------------------------------------------------------------------*/

