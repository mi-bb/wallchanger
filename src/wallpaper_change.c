/**
 * @file  wallpaper_change.c
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
 * @brief  Check settings and wallpaper change functions.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <sysexits.h>
#include <time.h>
#include "cfg_file.h"
#include "wallpaper_set.h"
#include "settings.h"
#include "errors.h"
#include "window_manager_info.h"
#include "wallpaper_change.h"
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
check_time_align_val (const uint32_t  change_interval,
                      uint32_t       *time_align_count)
{
    time_t     now;                /* Time for present */
    time_t     later;              /* Time for next full hour */
    struct tm *tm_now;             /* Struct for change */
    uint32_t   time_diff      = 0; /* Difference between now and next hour */
    uint32_t   time_align_val = 0; /* Time align value */

    time (&now);
    tm_now = localtime (&now);
#ifdef DEBUG
    printf ("%s\n", asctime (tm_now));
#endif
    tm_now->tm_hour += 1;
    tm_now->tm_min   = 0;
    tm_now->tm_sec   = 0;
#ifdef DEBUG
    printf ("%s\n", asctime (tm_now));
#endif
    later             = mktime (tm_now);
    time_diff         = (uint32_t) difftime (later, now);
    *time_align_count = time_diff / change_interval;
    time_align_val    = time_diff - (*time_align_count * change_interval);
#ifdef DEBUG
    printf ("difft %d\n", time_diff);
    printf ("atime %d\n", time_align_val);
    printf ("cnt   %d\n", *time_align_count);
#endif
    /* If interval is longer than hour set counter to intmax so
     * it should not get to zero and not count align time */
    if (change_interval > 3600)
        *time_align_count = UINT32_MAX;

    return time_align_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if a graphical session is present.
 *
 * Checks the WAYLAND_DISPLAY and DISPLAY environment variables, so it works
 * both under Wayland (without needing XWayland) and under X11, without
 * linking against either display library.
 *
 * @return 1 if a graphical session is present, 0 if not.
 */
static int
check_display (void)
{
    return (getenv ("WAYLAND_DISPLAY") != nullptr ||
            getenv ("DISPLAY")         != nullptr);
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
 * @brief  Look for config files in standard locations if config_file is null or
 *         check config_file path for existence. Exit on fail.
 */
void
check_config_file (char **config_file)
{
    int err_code = 0;
    if ((err_code = cfg_file_config_file_stuff (config_file, 0)) != ERR_OK)
        err (err_ex_code (err_code), "Problem with config file");
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free dynamic data and exit app.
 *
 * @param[out] config_file  String with config file path
 * @param[out] rand_mem     Random data
 * @param[out] settings     Settings data
 * @param[out] wm_info      Window manager info
 * @param[in]  exit_val     Exit return value
 * @param[in]  message      Message to show on exit or null
 * @return     none
 */
static void
free_and_exit (char       *config_file,
               RandMem    *rand_mem,
               Setting    *settings,
               Setting    *wm_info,
               int         exit_val,
               const char *message)
{
    free (config_file);
    randomm_free (rand_mem);
    settings_free_all (settings);
    settings_free_all (wm_info);
    if (message == nullptr) {
        exit (exit_val);
    }
    else {
        errx (exit_val, "%s", message);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Loads settings, checks for changes in wallpaper list length and
 *         changes wallpaper, returns change interval.
 */
uint32_t
chk_setts_ch_wall (char     *config_file,
                   RandMem  *rand_mem,
                   int      *time_align_option)
{
    static uint32_t  wallpaper_list_len = 0; /* Wallpaper list length */
    int              err_code           = 0; /* Error output */
    uint32_t         actual_len         = 0; /* Actual wallpaper list length */
    uint32_t         change_interval    = 0; /* Result change interval */
    Setting         *settings           = nullptr; /* For setting list */
    Setting         *wm_info            = nullptr; /* Window manager info */
    Setting         *setting_item       = nullptr; /* For particular setting */
    char            *cmd                = nullptr; /* For wallpaper change command */

    /* Read settings, check for empty config file, set defaults */
    settings = setts_read (config_file, &err_code);
    /* Get settings with window manager info */
    if (err_code == ERR_OK)
        wm_info = wms_get_wm_info (&err_code);

    if (err_code != ERR_OK) {
        free_and_exit (config_file, rand_mem, settings, wm_info,
                       err_ex_code (err_code), nullptr);
    }
    if (settings == nullptr) {
        free_and_exit (config_file, rand_mem, settings, wm_info,
                       EX_DATAERR, "Empty config file");
    }
    /* Check settings, set default values if some are missing */
    settings = setts_check_defaults (settings);

    /* Get number of wallpapers in list */
    actual_len = (uint32_t) setting_count_children (
            settings_find (settings, get_setting_name (SETT_WALL_ARRAY)));
    if (actual_len == 0) {
        free_and_exit (config_file, rand_mem, settings, wm_info,
                       EX_DATAERR, "Empty wallpaper list");
    }
    /* Get wallpaper set command depending on used window manager */
    cmd = wms_get_wallpaper_command (config_file, settings, wm_info, &err_code);
    /* Update wallpaper set command in settings */
    if (cmd != nullptr) {
        if ((setting_item = settings_find (
                        settings, get_setting_name (SETT_BG_CMD))) != nullptr) {
            setting_set_string (setting_item, cmd);
        }
        free (cmd);
    }
    /* Get time align info */
    setting_item = settings_find (settings, get_setting_name (SETT_TIME_ALIGN_OPT));
    if (setting_item != nullptr) {
        *time_align_option = (int) setting_get_int (setting_item);
    }
    /* Get wallpaper change interval value */
    setting_item = settings_find (settings, get_setting_name (SETT_INTERVAL_VAL));
    if (setting_item != nullptr) {
        change_interval = (uint32_t) setting_get_int (setting_item);
    }
    if (wallpaper_list_len == actual_len) {
        /* Wallpaper list length did not changed, change wallpaper */
        if ((err_code = wallpaper_set_change (settings, rand_mem, config_file)) != ERR_OK) {
            free_and_exit (config_file, rand_mem, settings, wm_info,
                           err_ex_code (err_code), nullptr);
        }
    }
    else {
        /* Wallpaper list length changed, reinit random, change wallpaper */
        randomm_set_range (rand_mem, (size_t) actual_len);

        if (wallpaper_list_len == 0) {
            /* Program startup, previous wallpaper count was 0 */
            err_code = wallpaper_set_startup_set (settings, rand_mem, config_file);
            if (err_code != ERR_OK) {
                free_and_exit (config_file, rand_mem, settings, wm_info,
                               err_ex_code (err_code), nullptr);
            }
        }
        else {
            /* Change during program work, previous wallpaper count was not 0 */
            if ((err_code = wallpaper_set_change (settings, rand_mem, config_file)) != ERR_OK) {
                free_and_exit (config_file, rand_mem, settings, wm_info,
                               err_ex_code (err_code), nullptr);
            }
        }
        wallpaper_list_len = actual_len;
    }
    settings_free_all (wm_info);
    settings_free_all (settings);

    return change_interval * 60;
}
/*----------------------------------------------------------------------------*/

