/**
 * @file  chkwch.c
 * @copyright Copyright (C) 2019-2021 Michał Bąbik
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
#include <sysexits.h>
#include <X11/Xlib.h>
#include <time.h>
#include "cfgfile.h"
#include "wpset.h"
#include "setts.h"
#include "errs.h"
#include "wmsfn.h"
#include "chkwch.h"
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
check_time_align_val (const uint32_t  ui_ch_int,
                      uint32_t       *ui_cnt)
{
    time_t     t_now;            /* Time for present */
    time_t     t_later;          /* Time for next full hour */
    struct tm *tm_now;           /* Struct for change */
    uint32_t   ui_difftime  = 0; /* Difference between now and next hour */
    uint32_t   ui_atime_val = 0; /* Time align value */

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
    t_later      = mktime (tm_now);
    ui_difftime  = (uint32_t) difftime (t_later, t_now);
    *ui_cnt      = ui_difftime / ui_ch_int;
    ui_atime_val = ui_difftime - (*ui_cnt * ui_ch_int);
#ifdef DEBUG
    printf ("difft %d\n", ui_difftime);
    printf ("atime %d\n", ui_atime_val);
    printf ("cnt   %d\n", *ui_cnt);
#endif
    /* If interval is longer than hour set counter to intmax so
     * it should not get to zero and not count align time */
    if (ui_ch_int > 3600)
        *ui_cnt = UINT32_MAX;

    return ui_atime_val;
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
    int i_err = 0;
    if ((i_err = cfgfile_config_file_stuff (s_file, 0)) != ERR_OK)
        err (err_ex_code (i_err), "Problem with config file");
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
/*----------------------------------------------------------------------------*/
/**
 * @brief  Loads settings, checks for changes in wallpaper list length and
 *         changes wallpaper, returns change interval.
 */
uint32_t
chk_setts_ch_wall (char     *s_cfg_file,
                   RandMem  *rm_rand,
                   int      *i_algntime)
{
    static uint32_t  ui_len   = 0;    /* Wallpaper list length */
    int              i_err    = 0;    /* Error output */
    uint32_t         ui_nlen  = 0;    /* Actual wallpaper list length */
    uint32_t         ui_inter = 0;    /* Result change interval */
    Setting         *st_c     = NULL; /* For setting list */
    Setting         *st_wm    = NULL; /* Window manager info */
    Setting         *st_st    = NULL; /* For particular setting */
    char            *s_cmd    = NULL; /* For wallpaper change command */

    /* Read settings, check for empty config file, set defaults */
    st_c = setts_read (s_cfg_file, &i_err);
    /* Get settings with window manager info */
    if (i_err == ERR_OK)
        st_wm = wms_get_wm_info (&i_err);

    if (i_err != ERR_OK) {
        free_and_exit (s_cfg_file, rm_rand, st_c, st_wm,
                       err_ex_code (i_err), NULL);
    }
    if (st_c == NULL) {
        free_and_exit (s_cfg_file, rm_rand, st_c, st_wm,
                       EX_DATAERR, "Empty config file");
    }
    /* Check settings, set default values if some are missing */
    setts_check_defaults (st_c);

    /* Get number of wallpapers in list */
    ui_nlen = (uint32_t) setting_count_children (
            settings_find (st_c, get_setting_name (SETT_WALL_ARRAY)));
    if (ui_nlen == 0) {
        free_and_exit (s_cfg_file, rm_rand, st_c, st_wm,
                       EX_DATAERR, "Empty wallpaper list");
    }
    /* Get wallpaper set command depending on used window manager */
    s_cmd = wms_get_wallpaper_command (s_cfg_file, st_c, st_wm, &i_err);
    /* Update wallpaper set command in settings */
    if (s_cmd != NULL) {
        if ((st_st = settings_find (
                        st_c, get_setting_name (SETT_BG_CMD))) != NULL) {
            setting_set_string (st_st, s_cmd);
        }
        free (s_cmd);
    }
    /* Get time align info */
    st_st = settings_find (st_c, get_setting_name (SETT_TIME_ALIGN_OPT));
    if (st_st != NULL) {
        *i_algntime = (int) setting_get_int (st_st);
    }
    /* Get wallpaper change inerval value */
    st_st = settings_find (st_c, get_setting_name (SETT_INTERVAL_VAL));
    if (st_st != NULL) {
        ui_inter = (uint32_t) setting_get_int (st_st);
    }
    if (ui_len == ui_nlen) {
        /* Wallpaper list length did not changed, change wallpaper */
        if ((i_err = wpset_change (st_c, rm_rand, s_cfg_file)) != ERR_OK) {
            free_and_exit (s_cfg_file, rm_rand, st_c, st_wm,
                           err_ex_code (i_err), NULL);
        }
    }
    else {
        /* Wallpaper list length changed, reinit random, change wallpaper */
        randomm_set_range (rm_rand, (size_t) ui_nlen);

        if (ui_len == 0) {
            /* Program startup, previous wallpaper count was 0 */
            i_err = wpset_startup_set (st_c, rm_rand, s_cfg_file);
            if (i_err != ERR_OK) {
                free_and_exit (s_cfg_file, rm_rand, st_c, st_wm,
                               err_ex_code (i_err), NULL);
            }
        }
        else {
            /* Change during progam work, previous wallpaper count was not 0 */
            if ((i_err = wpset_change (st_c, rm_rand, s_cfg_file)) != ERR_OK) {
                free_and_exit (s_cfg_file, rm_rand, st_c, st_wm,
                               err_ex_code (i_err), NULL);
            }
        }
        ui_len = ui_nlen;
    }
    settings_free_all (st_wm);
    settings_free_all (st_c);

    return ui_inter * 60;
}
/*----------------------------------------------------------------------------*/

