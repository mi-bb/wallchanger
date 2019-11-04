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
 * @retval     0 OK
 */
static int
wallpaper_set_file (const char *s_cmd,
                    const char *s_wall)
{
    char *s_cmdn = NULL;
    int   i_siz  = 0;

    i_siz = strlen (s_cmd) + strlen (s_wall) + 4;
    s_cmdn = g_malloc0 (i_siz * sizeof (char));
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
    GRand *gr_rand;
    char  *s_fn  = NULL;
    int    i_pos = 0;
    int    i_cnt = 0;

    i_cnt = g_slist_length (ws_sett->gsl_files);
    if (i_cnt == 0)
        return 0;

    gr_rand = g_rand_new ();

    i_pos = g_rand_int_range (gr_rand, 0, i_cnt);
    s_fn = g_slist_nth_data (ws_sett->gsl_files, i_pos);
    if (s_fn != NULL) {
        settings_set_last_used_data (ws_sett, s_fn);
        wallpaper_set_file (ws_sett->s_bgcmd, ws_sett->s_lastused);
    }
    g_rand_free (gr_rand);
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
    char *s_next = NULL;
    int   i_pos  = 0;

    // check empty list
    if (g_slist_length (ws_sett->gsl_files) == 0) {
        return 0;
    }
    if (ws_sett->s_lastused == NULL) {
        i_pos = 0;
        s_next = g_slist_nth_data (ws_sett->gsl_files, i_pos);
        settings_set_last_used_data (ws_sett, s_next);
        wallpaper_set_file (ws_sett->s_bgcmd, ws_sett->s_lastused);
        return 0;
    }
    i_pos = my_gslist_get_position (ws_sett->gsl_files,
                                    ws_sett->s_lastused) + 1;
    if (i_pos >= 0) {
        s_next = g_slist_nth_data (ws_sett->gsl_files, i_pos);
        if (s_next == NULL) {
            // lst one, get first
            s_next = g_slist_nth_data (ws_sett->gsl_files, 0);
        }
        if (s_next != NULL) {
            settings_set_last_used_data (ws_sett, s_next);
            wallpaper_set_file (ws_sett->s_bgcmd, ws_sett->s_lastused);
        }
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Wallpaper change during program work.
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Wallpaper change status
 */
int
wallpaper_change (WallSett *ws_sett)
{
    if (ws_sett->i_random) {
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
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Wallpaper set status
 */
int
wallpaper_startup_set (WallSett *ws_sett)
{
    int i_res = 0;
    if (ws_sett->i_lastsett) {
        wallpaper_set_file (ws_sett->s_bgcmd, ws_sett->s_lastused);
    }
    else {
        i_res = wallpaper_change (ws_sett);
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting wallpaper out of settings dialog.
 *
 * @param[in,out]  ws_sett  Program settings
 * @param[in]      s_file   Wallpaper file
 * @return         Wallpaper set status
 */
int
wallpaper_dialog_set (WallSett   *ws_sett,
                      const char *s_file)
{
    int i_res = 0;
    i_res = wallpaper_set_file (ws_sett->s_bgcmd, s_file);
    if (i_res)
        return i_res;
    i_res = settings_set_last_used_data (ws_sett, s_file);
    if (i_res)
        return i_res;
    i_res = settings_update_last_used (ws_sett);
    return i_res;
}
/*----------------------------------------------------------------------------*/

