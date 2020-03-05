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
 * @date March 06, 2020
 *
 * @version 1.3.11
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include "cfgfile.h"
#include "chkwch.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief Main function.
 * @param[in] argc Arguments passed to the program from the environment in
 *                 which the program is run
 * @param[in] argv Pointer to the first element of an array of pointers that
 *                 represent the arguments passed to the program
 * @return         Return value
 */
int
main (int    argc __attribute__ ((unused)),
      char **argv __attribute__ ((unused)))
{
    uint32_t  ui_cnt    = 0;    /* Minute counter */
    uint32_t  ui_ch_int = 0;    /* Change interval */
    char     *s_cfgfile = NULL; /* Config file path */

    check_display ();

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

