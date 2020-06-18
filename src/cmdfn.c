/**
 * @file  cmdfn.c
 * @copyright Copyright (C) 2019-2020 Michał Bąbik
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
 * @brief  Command line parsing functions
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "cmdline.h"
#include "cmdfn.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse command line options, set i_opt and s_cfgpath.
 */
void
cmdfn_parse (int    argc,
             char **argv,
             int   *i_opt,
             char **s_cfgpath __attribute__ ((unused)))
{
    struct gengetopt_args_info args_info;
    *i_opt = 0;

    if (cmdline_parser (argc, argv, &args_info) != 0)
        exit(EXIT_FAILURE);

    if (args_info.start_given && args_info.stop_given)
        errx (EXIT_FAILURE,
              "Start and stop options can't be mixed together");
    if (args_info.start_given && args_info.restart_given)
        errx (EXIT_FAILURE,
              "Start and restart options can't be mixed together");
    if (args_info.stop_given && args_info.restart_given)
        errx (EXIT_FAILURE,
              "Stop and restart options can't be mixed together");

    if (args_info.start_given) {
        *i_opt |= CMD_OPT_START;
    }
    if (args_info.stop_given) {
        *i_opt |= CMD_OPT_STOP;
    }
    if (args_info.restart_given) {
        *i_opt |= CMD_OPT_RESTART;
    }
    if (args_info.status_given) {
        *i_opt |= CMD_OPT_STATUS;
    }
    if (args_info.once_given) {
        *i_opt |= CMD_OPT_ONCE;
    }
    if (args_info.config_given &&
            !args_info.status_given && !args_info.stop_given) {
        *i_opt |= CMD_OPT_CONFIG;
        printf ("Config path %s\n", args_info.config_arg);
        *s_cfgpath = strdup (args_info.config_arg);
    }
    cmdline_parser_free (&args_info);
}
/*----------------------------------------------------------------------------*/

