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
 * @date April 11, 2020
 *
 * @version 1.4.0
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "cmdline.h"
#include "cfgfile.h"
#include "chkwch.h"
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
    uint32_t  ui_cnt    = 0;    /* Minute counter */
    uint32_t  ui_ch_int = 0;    /* Change interval */
    char     *s_cfgfile = NULL; /* Config file path */
    struct gengetopt_args_info args_info;

    if (cmdline_parser (argc, argv, &args_info) != 0)
        exit(EXIT_FAILURE);
    if (args_info.start_given && args_info.stop_given)
        errx (EXIT_FAILURE, "Start and stop options can't be mixed together");
    if (args_info.start_given && args_info.restart_given)
        errx (EXIT_FAILURE, "Start and restart options can't be mixed together");
    if (args_info.stop_given && args_info.restart_given)
        errx (EXIT_FAILURE, "Stop and restart options can't be mixed together");
    /* Printing status */
    if (args_info.status_given) {
        check_print_status ();
        exit(0);
    }
    /* Stopping daemon */
    if (args_info.stop_given) {
        check_daemon_kill ();
        exit(0);
    }
    /* Restarting daemon - stopping existing one */
    if (args_info.restart_given) {
        check_daemon_kill ();
    }
    /* Checking wchangerd process presence, exiting if it is running */
    check_daemon_exit ();
    /* Checking if display is available */
    check_display ();

    /* Starting daemon */
    if (args_info.start_given || args_info.restart_given) {
        printf ("Starting wchangerd daemon\n");

        int i_res = fork ();

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
        chdir ("/");
        for (long i = sysconf (_SC_OPEN_MAX); i >= 0; --i) {
            close ((int) i);
        }
    }

    s_cfgfile = cfgfile_get_config_path_exit ();
    ui_ch_int = check_settings_change_wallpaper (s_cfgfile);

    while (1) {

        if (++ui_cnt > ui_ch_int) {
            ui_ch_int = check_settings_change_wallpaper (s_cfgfile);
            ui_cnt = 1;
        }
        sleep (60);
    }
    free (s_cfgfile);
    return 0;
}
/*----------------------------------------------------------------------------*/

