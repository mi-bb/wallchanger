/**
 * @file  wallset.c
 * @copyright Copyright (C) 2019 Michał Bąbik
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
 * @brief  Wallpaper setting functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include "wallset.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set particular file as a wallpaper. 
 *
 * @param[in]  s_cmd   Wallpaper set command
 * @param[in]  s_wall  Wallpaper file
 * @return     Wallpaper set status
 */
static int
wallpaper_set_file (const char *s_cmd,
                    const char *s_wall)
{
    char  *s_cmdn = NULL; /* Wallpaper set command */
    size_t i_siz  = 0;    /* Size of wallpeper set command */

    i_siz = strlen (s_cmd) + strlen (s_wall) + 4;
    s_cmdn = calloc (i_siz, sizeof (char));
    if (s_cmdn == NULL) {
        fputs ("Alloc error\n", stderr);
        //return 1;
        exit (EXIT_FAILURE);
    }
    sprintf (s_cmdn, "%s %s &", s_cmd, s_wall);
    system (s_cmdn);
    free (s_cmdn);
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random image from list as a wallpaper.
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Wallpaper set status
 */
static int
wallpaper_set_random (WallSett *ws_sett)
{
    const char *s_fn  = NULL; /* Wallpaper file name */
    uint32_t    i_pos = 0;    /* Random wallpaper position */
    uint32_t    i_cnt = 0;    /* Length of wallpaper list */

    i_cnt = flist_get_len (&ws_sett->fl_files);
    if (i_cnt == 0)
        return 0;

    i_pos = randomm_get_number (&ws_sett->rm_mem);
    s_fn = flist_get_data (&ws_sett->fl_files, i_pos);

    if (s_fn != NULL) {
        settings_set_last_used_fn (ws_sett, s_fn);

        wallpaper_set_file (settings_get_command (ws_sett),
                            settings_get_last_used_fn (ws_sett));
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set next wallpaper from list.
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Wallpaper change status
 */
static int
wallpaper_set_next_in_list (WallSett *ws_sett)
{
    const char *s_next = NULL; /* Next wallpaper file name */
    int         i_pos  = 0;    /* Next wallpaper position in list */

    /* check empty list */
    if (flist_get_len (&ws_sett->fl_files) == 0) {
        return 0;
    }
    
    if (settings_get_last_used_fn (ws_sett) == NULL) {
        i_pos = 0;
        s_next = flist_get_data (&ws_sett->fl_files, i_pos);

        settings_set_last_used_fn (ws_sett, s_next);

        wallpaper_set_file (settings_get_command (ws_sett),
                            settings_get_last_used_fn (ws_sett));
        return 0;
    }
    i_pos = flist_get_pos (&ws_sett->fl_files,
                           settings_get_last_used_fn (ws_sett)) + 1;
    if (i_pos >= 0) {
        s_next = flist_get_data (&ws_sett->fl_files, i_pos);
        if (s_next == NULL) {
            /* lst one, get first */
            s_next = flist_get_data (&ws_sett->fl_files, 0);
        }
        if (s_next != NULL) {
            settings_set_last_used_fn (ws_sett, s_next);

            wallpaper_set_file (settings_get_command (ws_sett),
                                settings_get_last_used_fn (ws_sett));
        }
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Wallpaper change during program work.
 */
int
wallpaper_change (WallSett *ws_sett)
{
    if (settings_get_random (ws_sett)) {
        wallpaper_set_random (ws_sett);
    }
    else {
        wallpaper_set_next_in_list (ws_sett);
    }
    return settings_update_last_used (ws_sett);
    //return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting wallpaper image at program startup.
 */
int
wallpaper_startup_set (WallSett *ws_sett)
{
    int i_res = 0; /* Function result */

    if (settings_get_last_used_setting (ws_sett)) {
        wallpaper_set_file (settings_get_command (ws_sett),
                            settings_get_last_used_fn (ws_sett));
    }
    else {
        i_res = wallpaper_change (ws_sett);
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting wallpaper out of settings dialog.
 */
int
wallpaper_dialog_set (WallSett   *ws_sett,
                      const char *s_file)
{
    int i_res = 0;
    
    i_res = wallpaper_set_file (settings_get_command (ws_sett), s_file);
    if (i_res)
        return i_res;

    i_res = settings_set_last_used_fn (ws_sett, s_file);
    if (i_res)
        return i_res;

    i_res = settings_update_last_used (ws_sett);

    return i_res;
}
/*----------------------------------------------------------------------------*/

