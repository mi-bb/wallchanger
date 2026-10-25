/**
 * @file  cmd_functions.c
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
 * @brief  Command line parsing functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <sysexits.h>
#include "cmdline.h"
#include "cmd_functions.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse command line options, set options and config_path.
 */
void
cmd_parse (int    argc,
           char **argv,
           int   *options,
           char **config_path)
{
    struct gengetopt_args_info args_info;
    *options = 0;

    if (cmdline_parser (argc, argv, &args_info) != 0)
        exit (EX_USAGE);

    if (args_info.start_given && args_info.stop_given)
        errx (EX_USAGE, "Start and stop options can't be mixed together");
    if (args_info.start_given && args_info.restart_given)
        errx (EX_USAGE, "Start and restart options can't be mixed together");
    if (args_info.stop_given && args_info.restart_given)
        errx (EX_USAGE, "Stop and restart options can't be mixed together");

    if (args_info.start_given) {
        *options |= CMD_OPT_START;
    }
    if (args_info.stop_given) {
        *options |= CMD_OPT_STOP;
    }
    if (args_info.restart_given) {
        *options |= CMD_OPT_RESTART;
    }
    if (args_info.status_given) {
        *options |= CMD_OPT_STATUS;
    }
    if (args_info.once_given) {
        *options |= CMD_OPT_ONCE;
    }
    if (args_info.config_given &&
            !args_info.status_given && !args_info.stop_given) {
        *options |= CMD_OPT_CONFIG;
        printf ("Config path %s\n", args_info.config_arg);
        *config_path = strdup (args_info.config_arg);
    }
    cmdline_parser_free (&args_info);
}
/*----------------------------------------------------------------------------*/

