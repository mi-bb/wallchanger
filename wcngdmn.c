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
 * Automatic wallpaper changer
 *
 * @date November 25, 2019
 *
 * @version 1.3.0
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
 * @param[in] argc Arguments passed to the program from the environment in
 *                 which the program is run
 * @param[in] argv Pointer to the first element of an array of pointers that
 *                 represent the arguments passed to the program
 * @return         Return value
 */  
int
main (int    argc,
      char **argv)
{
    SettList *st_list;
    WallSett  ws_sett;
    uint32_t  ui_cnt = 0;
    int       i_err  = 0;

    if (wallset_init (&ws_sett) != ERR_OK) {
        wallset_free (&ws_sett);
        exit (EXIT_FAILURE);
    }

    st_list = settings_read (ws_sett.s_cfgfile, &i_err);
    if (i_err != ERR_OK) {
        stlist_free (st_list);
        wallset_free (&ws_sett);
        exit (EXIT_FAILURE);
    }

    settlist_to_wallset (st_list, &ws_sett);
    stlist_free (st_list);

    if (flist_get_len (&ws_sett.fl_files) == 0) {
        fputs ("Empty file list\n", stderr);
        wallset_free (&ws_sett);
        exit (EXIT_FAILURE);
    }

    /* Set the maximun random range to the length of the file list */
    randomm_set_range (&ws_sett.rm_mem, flist_get_len (&ws_sett.fl_files));

    if (wallpaper_startup_set (&ws_sett) != ERR_OK) {
        wallset_free (&ws_sett);
        exit (EXIT_FAILURE);
    }

    while (1) {
        sleep (60);
        ui_cnt++;
        if (ui_cnt >= ws_sett.i_chinterval) {
            if (wallpaper_change (&ws_sett) != ERR_OK) {
                wallset_free (&ws_sett);
                exit(EXIT_FAILURE);
            }
            ui_cnt = 0;
        }
    }
    wallset_free (&ws_sett);

    return 0;
}
/*----------------------------------------------------------------------------*/

