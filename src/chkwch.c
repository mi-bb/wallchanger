/**
 * @file  chkwch.c
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
 * @brief  Check settings and wallpaper change functions.
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <X11/Xlib.h>
#include <time.h>
#include "cfgfile.h"
#include "wpset.h"
#include "setts.h"
#include "errs.h"
#include "chkwch.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sleep for 500 milliseconds.
 */
void sleep500 (void)
{
    struct timespec ts = {0, 500000000};

    nanosleep(&ts, NULL);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if display is present, exit if it is not.
 */
#include <stdio.h>
void
check_display_exit (void)
{
    Display *display;

    display = XOpenDisplay (NULL);
    if (display == NULL) {
        errx (EXIT_FAILURE, "Could not detect display");
    }
    XCloseDisplay(display);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Look for config files in standard locations if s_file is null or
 *         check s_file path for existence. Exit on fail.
 */
void
check_config_file (char **s_file)
{
    if (cfgfile_config_file_stuff (s_file, 0) != ERR_OK) {
        exit (EXIT_FAILURE);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Loads settings, checks for changes in wallpaper list length and
 *         changes wallpaper, returns change interval.
 */
uint32_t
check_settings_change_wallpaper (char    *s_cfg_file,
                                 RandMem *rm_rand)
{
    static uint32_t  ui_len  = 0; /* Wallpaper list length */
    int              i_err   = 0; /* Error output */
    uint32_t         ui_nlen = 0; /* Actual wallpaper list length */
    uint32_t         ui_res  = 0; /* Result change interval */
    SettList        *st_list;     /* Setting list */
    WallSett        *ws_sett;     /* WallSett object with settings for wallpaper
                                     change functions */
    ws_sett = wallset_new (rm_rand);
    wallset_set_cfgfile (ws_sett, s_cfg_file);
    st_list = settings_read (s_cfg_file, &i_err);

    if (i_err != ERR_OK) {
        stlist_free (st_list);
        free (ws_sett);
        free (s_cfg_file);
        exit (EXIT_FAILURE);
    }
    if (stlist_get_length (st_list) == 0) {
        stlist_free (st_list);
        free (ws_sett);
        free (s_cfg_file);
        errx (EXIT_FAILURE, "Empty config file");
    }

    settlist_check_defaults (st_list);
    settlist_to_wallset (st_list, ws_sett);
    ui_nlen = (uint32_t) stlist_get_length (
            wallset_get_wallpaper_list (ws_sett));

    ui_res = wallset_get_interval (ws_sett);

    if (ui_nlen == 0) {
        /* Empty wallpaper list, free and exit */
        stlist_free (st_list);
        wallset_free (ws_sett);
        free (s_cfg_file);
        errx (EXIT_FAILURE, "Empty wallpaper list");
    }
    else if (ui_len == ui_nlen) {
        /* Wallpaper list length not changed, change wallpaper */
        if (wallpaper_change (ws_sett) != ERR_OK) {
            stlist_free (st_list);
            wallset_free (ws_sett);
            free (s_cfg_file);
            exit(EXIT_FAILURE);
        }
    }
    else {
        /* Wallpaper list length changed, reinit random, change wallpaper */
        randomm_init (ws_sett->rm_rand);
        randomm_set_range (ws_sett->rm_rand, (int32_t) ui_nlen);

        if (ui_len == 0) {
            /* Program startup */
            if (wallpaper_startup_set (ws_sett) != ERR_OK) {
                stlist_free (st_list);
                wallset_free (ws_sett);
                free (s_cfg_file);
                exit (EXIT_FAILURE);
            }
        }
        else {
            /* Change during progam work */
            if (wallpaper_change (ws_sett) != ERR_OK) {
                stlist_free (st_list);
                wallset_free (ws_sett);
                free (s_cfg_file);
                exit (EXIT_FAILURE);
            }
        }
        ui_len = ui_nlen;
    }
    stlist_free (st_list);
    wallset_free (ws_sett);
    return ui_res;
}
/*----------------------------------------------------------------------------*/

