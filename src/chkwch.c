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
 * @author Michal Babik <michal.babik@pm.me>
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
#include "wmsfn.h"
#include "chkwch.h"
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
#include <stdio.h>
#endif
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
void
print_now (void)
{
    time_t     t_now;
    struct tm *tm_now;

    time (&t_now);
    tm_now = localtime (&t_now);
    printf ("%s\n", asctime (tm_now));
}
#endif
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count time to align intervals to next full hour.
 */
uint32_t
check_time_align_val (void)
{
    time_t     t_now;   /* Time for present */
    time_t     t_later; /* Time for next full hour */
    struct tm *tm_now;  /* Struct for change */

    time (&t_now);
    tm_now = localtime (&t_now);
    #ifdef DEBUG
    printf ("%s\n", asctime (tm_now));
    #endif
    tm_now->tm_hour += 1;
    tm_now->tm_min   = 0;
    tm_now->tm_sec   = 0;
    #ifdef DEBUG
    printf ("%s\n", asctime (tm_now));
    #endif
    t_later = mktime (tm_now);
    return (uint32_t) difftime (t_later, t_now);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if display is present, exit if it is not.
 *
 * @return none
 */
static int
check_display (void)
{
    Display *display;

    if ((display = XOpenDisplay (NULL)) == NULL)
        return 0;
    XCloseDisplay(display);
    return 1;
}
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
void
check_display_exit (void)
{
    if (!check_display ())
        errx (EXIT_FAILURE, "Could not detect display");
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Look for config files in standard locations if s_file is null or
 *         check s_file path for existence. Exit on fail.
 */
void
check_config_file (char **s_file)
{
    int i_err = 0; /* Error output */

    if ((i_err = cfgfile_config_file_stuff (s_file, 0)) != ERR_OK) {
        err (EXIT_FAILURE, "Problem with config file");
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free dynamic data and exit app.
 *
 * @param[out] s_cfg_file  String with config file path
 * @param[out] rm_rand     Random data
 * @param[out] st_settings Settings data
 * @param[out] st_wminfo   Window manager info
 * @param[in]  i_exit_val  Exit return value
 * @param[in]  s_message   Message to show on exit or null
 * @return     none
 */
static void
free_and_exit (char       *s_cfg_file,
               RandMem    *rm_rand,
               Setting    *st_settings,
               Setting    *st_wminfo,
               int         i_exit_val,
               const char *s_message)
{
    free (s_cfg_file);
    randomm_free (rm_rand);
    settings_free_all (st_settings);
    settings_free_all (st_wminfo);
    if (s_message == NULL) {
        exit (i_exit_val);
    }
    else {
        errx (i_exit_val, "%s", s_message);
    }
}
#include <stdio.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Loads settings, checks for changes in wallpaper list length and
 *         changes wallpaper, returns change interval.
 */
uint32_t
check_settings_change_wallpaper (char     *s_cfg_file,
                                 RandMem  *rm_rand,
                                 int      *i_algntime)
{
    static uint32_t  ui_len   = 0;    /* Wallpaper list length */
    int              i_err    = 0;    /* Error output */
    uint32_t         ui_nlen  = 0;    /* Actual wallpaper list length */
    uint32_t         ui_inter = 0;    /* Result change interval */
    Setting         *st_setts = NULL; /* For settings */
    Setting         *st_wm    = NULL; /* Window manager info */
    Setting         *st_st    = NULL; /* For particular setting */
    char            *s_cmd    = NULL; /* For wallpaper change command */

    /* Read settings, check for empty config file, set defaults */
    st_setts = setts_read (s_cfg_file, &i_err);
    /* Get settings with window manager info */
    if (i_err == ERR_OK)
        st_wm = wms_get_wm_info (&i_err);

    if (i_err != ERR_OK) {
        free_and_exit (s_cfg_file, rm_rand, st_setts, st_wm,
                       EXIT_FAILURE, NULL);
    }
    if (setting_get_child (st_setts) == NULL) {
        free_and_exit (s_cfg_file, rm_rand, st_setts, st_wm,
                       EXIT_FAILURE, "Empty config file");
    }
    /* Check settings, set default values if some are missing */
    setts_check_defaults (st_setts);

    /* Get number of wallpapers in list */
    ui_nlen = (uint32_t) setting_count_children (
            settings_find (setting_get_child (st_setts),
                           get_setting_name (SETTING_WALL_ARRAY)));
    if (ui_nlen == 0) {
        free_and_exit (s_cfg_file, rm_rand, st_setts, st_wm,
                       EXIT_FAILURE, "Empty wallpaper list");
    }
    /* Get wallpaper set command depending on used window manager */
    s_cmd = wms_get_wallpaper_command (s_cfg_file, setting_get_child (st_setts),
                                       setting_get_child (st_wm), &i_err);
    /* Update wallpaper set command in settings */
    if (s_cmd != NULL) {
        if ((st_st = setting_find_child (st_setts,
                    get_setting_name (SETTING_BG_CMD))) != NULL) {
            setting_set_string (st_st, s_cmd);
        }
        free (s_cmd);
    }
    /* Get time align info */
    st_st = settings_find (setting_get_child (st_setts),
                           get_setting_name (SETTING_TIME_ALIGN_OPT));
    if (st_st != NULL) {
        *i_algntime = (int) setting_get_int (st_st);
    }
    /* Get wallpaper change inerval value */
    st_st = settings_find (setting_get_child (st_setts),
                           get_setting_name (SETTING_INTERVAL_VAL));
    if (st_st != NULL) {
        ui_inter = (uint32_t) setting_get_int (st_st);
    }
    if (ui_len == ui_nlen) {
        /* Wallpaper list length did not changed, change wallpaper */
        if (wpset_change (st_setts, rm_rand, s_cfg_file) != ERR_OK) {
            free_and_exit (s_cfg_file, rm_rand, st_setts, st_wm,
                           EXIT_FAILURE, NULL);
        }
    }
    else {
        /* Wallpaper list length changed, reinit random, change wallpaper */
        randomm_init (rm_rand);
        randomm_set_range (rm_rand, (int32_t) ui_nlen);

        if (ui_len == 0) {
            /* Program startup, previous wallpaper count was 0 */
            if (wpset_startup_set (st_setts, rm_rand, s_cfg_file) != ERR_OK) {
                free_and_exit (s_cfg_file, rm_rand, st_setts, st_wm,
                               EXIT_FAILURE, NULL);
            }
        }
        else {
            /* Change during progam work, previous wallpaper count was not 0 */
            if (wpset_change (st_setts, rm_rand, s_cfg_file) != ERR_OK) {
                free_and_exit (s_cfg_file, rm_rand, st_setts, st_wm,
                               EXIT_FAILURE, NULL);
            }
        }
        ui_len = ui_nlen;
    }
    //setting_remove (st_wm);
    //setting_remove (st_wm);
    //st_wm = NULL;
    setting_remove (setting_get_child (st_wm));
    settings_free_all (st_wm);
    settings_free_all (st_setts);
    return ui_inter * 60;
}
/*----------------------------------------------------------------------------*/

