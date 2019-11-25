/**
 * @file  setts.c
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
 * @brief  Settings manage functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include "setts.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get name of setting in config file, based on enum value.
 */
const char *
get_setting_name (int i_val)
{
    const char *s_res = NULL;

    switch (i_val) {
        case SETTING_BG_CMD:
            s_res = "Background set command";
            break;

        case SETTING_LAST_USED_OPT:
            s_res = "Set last used wallpaper";
            break;
        
        case SETTING_LAST_USED_STR:
            s_res = "Last used wallpaper file";
            break;

        case SETTING_LAST_USED_POS:
            s_res = "Last used wallpaper pos";
            break;

        case SETTING_WIN_WIDTH:
            s_res = "Window width";
            break;

        case SETTING_WIN_HEIGHT:
            s_res = "Window height";
            break;

        case SETTING_RANDOM_OPT:
            s_res = "Random wallpaper";
            break;

        case SETTING_INTERVAL_VAL:
            s_res = "Wallpaper change interval";
            break;

        case SETTING_WALL_ARRAY:
            s_res = "Backgrounds";
            break;

        default:
            break;
    }
    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read program settings. 
 */
SettList *
settings_read (const char *s_fname,
               int        *i_err)
{
    return js_settings_read (s_fname, i_err);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert settings in SettList format to WallSett format.
 */
void
settlist_to_wallset (SettList *st_list,
                     WallSett *ws_sett)
{
    Setting    *st_sett;
    SettList   *sl_walls;
    const char *s_tmp  = NULL;
    const char *s_fn   = NULL;
    uint32_t    ui_tmp = 0;
    uint32_t    ui_cnt = 0;
    uint32_t    i      = 0;

    st_sett = stlist_get_setting_with_name (
            st_list, get_setting_name (SETTING_BG_CMD));
    if (st_sett != NULL) {
        s_tmp = setting_get_string (st_sett);
        wallset_set_command (ws_sett, s_tmp);
    }

    st_sett = stlist_get_setting_with_name (
            st_list, get_setting_name (SETTING_LAST_USED_STR));
    if (st_sett != NULL) {
        s_tmp = setting_get_string (st_sett);
        wallset_set_last_used_fn (ws_sett, s_tmp);
    }

    st_sett = stlist_get_setting_with_name (
            st_list, get_setting_name (SETTING_RANDOM_OPT));
    if (st_sett != NULL) {
        ui_tmp = setting_get_uint32 (st_sett);
        wallset_set_random (ws_sett, (uint8_t) ui_tmp);
    }

    st_sett = stlist_get_setting_with_name (
            st_list, get_setting_name (SETTING_LAST_USED_OPT));
    if (st_sett != NULL) {
        ui_tmp = setting_get_uint32 (st_sett);
        wallset_set_last_used_setting (ws_sett, (uint8_t) ui_tmp);
    }

    st_sett = stlist_get_setting_with_name (
            st_list, get_setting_name (SETTING_INTERVAL_VAL));
    if (st_sett != NULL) {
        ui_tmp = setting_get_uint32 (st_sett);
        wallset_set_interval (ws_sett, ui_tmp);
    }

    sl_walls = stlist_get_settings_in_array_name (st_list,
            get_setting_name (SETTING_WALL_ARRAY));

    ui_cnt = stlist_get_length (sl_walls);

    for (i = 0; i < ui_cnt; ++i) {
        st_sett = stlist_get_setting_at_pos (sl_walls, i);
        s_fn = setting_get_string (st_sett);
        if (s_fn != NULL) {
            flist_insert_data (&ws_sett->fl_files, s_fn);
            }
    }
    stlist_free (sl_walls);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update last used wallpaper position in config file. 
 */
int
settings_update_last_used (const char *s_last_used,
                           const char *s_fname)
{
    SettList *st_list;
    Setting  *st_sett;
    int       i_res = ERR_OK;

    st_list = stlist_new_list ();
    st_sett = setting_new_string (s_last_used,
                                  get_setting_name (SETTING_LAST_USED_STR));
    stlist_insert_setting (st_list, st_sett);
    i_res = js_settings_check_update_file (st_list, s_fname);
    stlist_free (st_list);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update window size in config file. 
 */
int
settings_update_window_size (const int   i_w,
                             const int   i_h,
                             const char *s_cfg_file)
{
    SettList *st_list;
    Setting  *st_sett;
    int       i_res = ERR_OK;

    st_list = stlist_new_list ();
    st_sett = setting_new_uint32 (i_w, get_setting_name (SETTING_WIN_WIDTH));
    stlist_insert_setting (st_list, st_sett);
    st_sett = setting_new_uint32 (i_h, get_setting_name (SETTING_WIN_HEIGHT));
    stlist_insert_setting (st_list, st_sett);
    i_res = js_settings_check_update_file (st_list, s_cfg_file);
    stlist_free (st_list);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings in SettList are an update to settings
 *         stored in settings file.
 */
int
settings_check_update (SettList    *st_list,
                       const char  *s_fname,
                       char       **s_out_buff)
{
    return  js_settings_check_for_update (st_list, s_fname, s_out_buff);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update file with new data.
 */
int
settings_update_file (const char *s_buff, 
                      const char *s_fname)
{
    return js_settings_update_file (s_buff, s_fname);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings are an update and update file with new data
 *         if they are.
 */
int
settings_check_update_file (SettList   *st_list,
                            const char *s_fname)
{
    return js_settings_check_update_file (st_list, s_fname);
}
/*----------------------------------------------------------------------------*/

