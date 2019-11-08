/** 
 * @file  wcngdmn.c
 * @copyright Copyright (C) 2019 Michał Bąbik
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
 * Program to change wallpapers.
 *
 * @date November 8, 2019
 *
 * @version 1.1.1
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include "setts.h"
#include "settstr.h"
#include "wallset.h"

#include "miscfun.h"
/*----------------------------------------------------------------------------*/
int
main (int    argc,
      char **argv)
{
    WallSett  ws_sett;
    uint32_t  i_cnt = 0;

    if (settings_init (&ws_sett)) {
        free_wall_sett (&ws_sett);
        exit (1);
    }
    if (settings_read (&ws_sett) != 0) {
        free_wall_sett (&ws_sett);
        exit (1);
    }
    if (g_slist_length (ws_sett.gsl_files) == 0) {
        fputs ("Empty file list\n", stderr);
        free_wall_sett (&ws_sett);
        exit (1);
    }

    if (wallpaper_startup_set (&ws_sett) > 0) {
        free_wall_sett (&ws_sett);
        exit (1);
    }

    while (1) {
        g_usleep (G_USEC_PER_SEC * 60);
        i_cnt++;
        if (i_cnt >= ws_sett.i_chinterval) {
            if (wallpaper_change (&ws_sett) > 0) {
                free_wall_sett (&ws_sett);
                exit(1);
            }
            i_cnt = 0;
        }
    }
    free_wall_sett (&ws_sett);
    return 0;
}
/*----------------------------------------------------------------------------*/

