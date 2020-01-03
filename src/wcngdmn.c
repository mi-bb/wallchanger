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
 * @date December 22, 2019
 *
 * @version 1.3.5
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "setts.h"
#include "wallsett.h"
#include "wpset.h"
#include "rwdt.h"
#include "randomm.h"
#include "settlist.h"
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
main (int    argc __attribute__ ((unused)),
      char **argv __attribute__ ((unused)))
{
    SettList *st_list;
    WallSett  ws_sett;
    uint32_t  ui_cnt  = 0;
    uint32_t  ui_len  = 0;
    uint32_t  ui_nlen = 0;
    int       i_err   = 0;

    if (wallset_init (&ws_sett) != ERR_OK) {
        exit (EXIT_FAILURE);
    }

    st_list = settings_read (ws_sett.s_cfgfile, &i_err);

    if (i_err != ERR_OK) {
        stlist_free (st_list);
        wallset_free (&ws_sett);
        exit (EXIT_FAILURE);
    }

    if (stlist_get_length (st_list) == 0) {
        fputs ("Empty config file\n", stderr);
        stlist_free (st_list);
        exit (EXIT_FAILURE);
    }

    settlist_check_defaults (st_list);

    settlist_to_wallset (st_list, &ws_sett);

    ui_len = stlist_get_length (ws_sett.sl_walls);

    if (ui_len == 0) {
        fputs ("Empty wallpaper list\n", stderr);
        stlist_free (st_list);
        wallset_free (&ws_sett);
        exit (EXIT_FAILURE);
    }

    /* Set the maximun random range to the length of the file list */
    randomm_set_range (&ws_sett.rm_mem, ui_len);

    if (wallpaper_startup_set (&ws_sett) != ERR_OK) {
        stlist_free (st_list);
        wallset_free (&ws_sett);
        exit (EXIT_FAILURE);
    }

    stlist_free (st_list);
    wallset_free (&ws_sett);

    while (1) {

        sleep (60);
        ui_cnt++;

        if (ui_cnt >= ws_sett.i_chinterval) {

            st_list = settings_read (ws_sett.s_cfgfile, &i_err);

            if (i_err != ERR_OK) {
                stlist_free (st_list);
                wallset_free (&ws_sett);
                exit (EXIT_FAILURE);
            }

            if (stlist_get_length (st_list) == 0) {
                fputs ("Empty config file\n", stderr);
                stlist_free (st_list);
                exit (EXIT_FAILURE);
            }

            settlist_check_defaults (st_list);

            settlist_to_wallset (st_list, &ws_sett);

            ui_nlen = stlist_get_length (ws_sett.sl_walls);

            if (ui_nlen == 0) {
                fputs ("Empty wallpaper list\n", stderr);
                stlist_free (st_list);
                wallset_free (&ws_sett);
                exit (EXIT_FAILURE);
            }

            if (ui_nlen != ui_len) {
                randomm_init (&ws_sett.rm_mem);
                randomm_set_range (&ws_sett.rm_mem, ui_nlen);
                ui_len = ui_nlen;
            }

            if (wallpaper_change (&ws_sett) != ERR_OK) {
                stlist_free (st_list);
                wallset_free (&ws_sett);
                exit(EXIT_FAILURE);
            }

            stlist_free (st_list);
            wallset_free (&ws_sett);
            ui_cnt = 0;
        }
    }

    return 0;
}
/*----------------------------------------------------------------------------*/

