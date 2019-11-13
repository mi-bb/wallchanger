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
 * @date November 14, 2019
 *
 * @version 1.2
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <unistd.h>
#include <stdlib.h>
#include "setts.h"
#include "settstr.h"
#include "wallset.h"
#include "miscfun.h"
#include "flist.h"
#include "randomm.h"
#include "errs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief Main function.
 *
 * @param[in] argc Arguments passed to the program from the environment in which
 *                 the program is run
 * @param[in] argv Pointer to the first element of an array of pointers that
 *                 represent the arguments passed to the program
 * @return         Return value
 */  
int
main (int    argc,
      char **argv)
{
    WallSett  ws_sett;
    uint32_t  ui_cnt = 0;
    
    if (settings_init (&ws_sett)) {
        settings_free (&ws_sett);
        exit (EXIT_FAILURE);
    }
    if (settings_read (&ws_sett) != ERR_OK) {
        settings_free (&ws_sett);
        exit (EXIT_FAILURE);
    }
    if (flist_get_len (&ws_sett.fl_files) == 0) {
        fputs ("Empty file list\n", stderr);
        settings_free (&ws_sett);
        exit (EXIT_FAILURE);
    }

    randomm_set_range (&ws_sett.rm_mem, flist_get_len (&ws_sett.fl_files));

    if (wallpaper_startup_set (&ws_sett) != ERR_OK) {
        settings_free (&ws_sett);
        exit (EXIT_FAILURE);
    }

    while (1) {
        sleep (60);
        ui_cnt++;
        if (ui_cnt >= ws_sett.i_chinterval) {
            if (wallpaper_change (&ws_sett) != ERR_OK) {
                settings_free (&ws_sett);
                exit(EXIT_FAILURE);
            }
            ui_cnt = 0;
        }
    }
    settings_free (&ws_sett);

    return 0;
}
/*----------------------------------------------------------------------------*/

