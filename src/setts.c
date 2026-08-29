/**
 * @file  setts.c
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
 * @brief  Settings manage functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdint.h>

#include <stdio.h>
#include <string.h>
#include "jsfun.h"
#include "setting.h"
#include "setts.h"
#include "errs.h"
#include "defs.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct SettingData
 *
 * @brief  Structure with Setting item info for checking default values.
 *
 * @var   SettingData::setting_id
 * @brief Setting enum id number
 *
 * @var   SettingData::setting_type
 * @brief Setting type SetValType value
 *
 * @var   SettingData::default_int
 * @brief Default value for integer type
 *
 * @var   SettingData::default_double
 * @brief Default value for double type
 *
 * @var   SettingData::default_string
 * @brief Default value for a string
 *
 */
typedef struct
SettingData {
    int        setting_id;
    SetValType setting_type;
    int64_t    default_int;
    double     default_double;
    char       default_string[128];
} SettingData;

/*----------------------------------------------------------------------------*/
/**
 * @brief  Get name of setting in config file, based on enum value.
 */
const char *
get_setting_name (const int i_val)
{
    const char *s_res = nullptr;

    switch (i_val) {
        case SETT_BG_CMD:
            s_res = "Background set command";
            break;

        case SETT_LAST_USED_OPT:
            s_res = "Set last used wallpaper";
            break;
        
        case SETT_LAST_USED_STR:
            s_res = "Last used wallpaper file";
            break;

        case SETT_LAST_USED_WM:
            s_res = "Last used window manager";
            break;

        case SETT_WIN_WIDTH:
            s_res = "Window width";
            break;

        case SETT_WIN_HEIGHT:
            s_res = "Window height";
            break;

        case SETT_RANDOM_OPT:
            s_res = "Random wallpaper";
            break;

        case SETT_INTERVAL_VAL:
            s_res = "Wallpaper change interval";
            break;

        case SETT_TIME_ALIGN_OPT:
            s_res = "Time align";
            break;

        case SETT_WALL_ARRAY:
            s_res = "Backgrounds";
            break;

        case SETT_WEB_DLG_WIDTH:
            s_res = "Web dilaog width";
            break;

        case SETT_WEB_DLG_HEIGHT:
            s_res = "Web dilaog height";
            break;

        case SETT_THUMB_QUALITY:
            s_res = "Thumbnail quality";
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
Setting *
setts_read (const char *s_cfg_file,
            int        *i_err)
{
    return js_settings_read (s_cfg_file, i_err);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check for a Setting with name as in sd_data presence and insert
 *         new one with data as in sd_data.
 *
 * @param[in] st_settings  List of Setting items
 * @param[in] sd_data      Data with setting info to examine
 * @return    List of Setting items, with new one appended if it was
 *            missing. Must be used, as it may differ from st_settings
 *            if the list was empty.
 */
static Setting *
setts_check_setting (Setting           *st_settings,
                     const SettingData *sd_data)
{
    const char *s_name = nullptr;
    Setting    *st_sett;

    s_name = get_setting_name (sd_data->setting_id);

    if ((st_sett = settings_find (st_settings, s_name)) == nullptr) {
#ifdef DEBUG
        printf ("%s", s_name);
        printf (" not present, setting default ");
#endif
        if (sd_data->setting_type == SET_VAL_INT) {
#ifdef DEBUG
            printf ("%" PRId64 "\n", sd_data->default_int);
#endif
            st_settings = settings_append (st_settings,
                    setting_new_int (s_name, sd_data->default_int));
        }
        else if (sd_data->setting_type == SET_VAL_DOUBLE) {
#ifdef DEBUG
            printf ("%f\n", sd_data->default_double);
#endif
            st_settings = settings_append (st_settings,
                    setting_new_double (s_name, sd_data->default_double));
        }
        else if (sd_data->setting_type == SET_VAL_STRING) {
#ifdef DEBUG
            printf ("%s\n", sd_data->default_string);
#endif
            st_settings = settings_append (st_settings,
                    setting_new_string (s_name, sd_data->default_string));
        }
        else if (sd_data->setting_type == SET_VAL_ARRAY) {
#ifdef DEBUG
            printf ("\n");
#endif
            st_settings = settings_append (st_settings,
                          setting_new_array (s_name));
        }
    }
    else {
#ifdef DEBUG
        printf ("%s", s_name);
        printf (" OK\n");
#endif
    }

    return st_settings;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check Setting list values and set default ones if needed.
 */
Setting *
setts_check_defaults (Setting *st_settings)
{
    SettingData *sd_data;

    SettingData sdd[] = {
        {SETT_LAST_USED_OPT,  SET_VAL_INT,    DEFAULT_LAST_USED_OPT, 0, ""},
        {SETT_LAST_USED_WM,   SET_VAL_STRING, 0, 0, DEFAULT_LAST_USED_WM},
        {SETT_RANDOM_OPT,     SET_VAL_INT,    DEFAULT_RANDOM_OPT, 0, ""},
        {SETT_TIME_ALIGN_OPT, SET_VAL_INT,    DEFAULT_TIME_ALIGN_OPT, 0, ""},
        {SETT_WIN_WIDTH,      SET_VAL_INT,    DEFAULT_WIN_WIDTH, 0, ""},
        {SETT_WIN_HEIGHT,     SET_VAL_INT,    DEFAULT_WIN_HEIGHT, 0, ""},
        {SETT_INTERVAL_VAL,   SET_VAL_INT,    DEFAULT_INTERVAL_VAL, 0, ""},
        {SETT_BG_CMD,         SET_VAL_STRING, 0, 0, DEFAULT_BG_CMD},
        {SETT_WALL_ARRAY,     SET_VAL_ARRAY,  0, 0, ""},
        {SETT_WEB_DLG_WIDTH,  SET_VAL_INT,    DEFAULT_WEB_DLG_WIDTH, 0, ""},
        {SETT_WEB_DLG_HEIGHT, SET_VAL_INT,    DEFAULT_WEB_DLG_HEIGHT, 0, ""},
        {SETT_THUMB_QUALITY,  SET_VAL_INT,    DEFAULT_THUMB_QUALITY, 0, ""},
        {-1, 0, 0, 0, ""}
    };

    for (sd_data = sdd; sd_data->setting_id != -1; ++sd_data) {
        st_settings = setts_check_setting (st_settings, sd_data);
    }

    return st_settings;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update last used wallpaper position in config file. 
 */
int
setts_update_last_used (const char *s_cfg_file,
                        const char *s_last_used)
{
    Setting  *st_settings;
    int       i_res = ERR_OK;

    st_settings = setting_new_string (get_setting_name (SETT_LAST_USED_STR),
                                      s_last_used);

    i_res = js_settings_check_update_file (st_settings, s_cfg_file);
    settings_free_all (st_settings);
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update last used window manager value in config file. 
 */
int
setts_update_last_used_wm (const char *s_cfg_file,
                           const char *s_last_used_wm)
{
    Setting  *st_settings;
    int       i_res = ERR_OK;

    st_settings = setting_new_string (get_setting_name (SETT_LAST_USED_WM),
                                      s_last_used_wm);

    i_res = js_settings_check_update_file (st_settings, s_cfg_file);
    settings_free_all (st_settings);
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update window size in config file. 
 */
int
setts_update_window_size (const char *s_cfg_file,
                          const int   i_w,
                          const int   i_h)
{
    Setting  *st_settings = nullptr;
    int       i_res = ERR_OK;

    st_settings = settings_append (st_settings,
                     setting_new_int (get_setting_name (SETT_WIN_WIDTH),
                                      (int64_t) i_w));
    st_settings = settings_append (st_settings,
                     setting_new_int (get_setting_name (SETT_WIN_HEIGHT),
                                      (int64_t) i_h));

    i_res = js_settings_check_update_file (st_settings, s_cfg_file);
    settings_free_all (st_settings);
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update wallpaper from web dialog size in config file.
 */
int
setts_update_web_dlg_size (const char *s_cfg_file,
                           const int   i_w,
                           const int   i_h)
{
    Setting  *st_settings = nullptr;
    int       i_res = ERR_OK;

    st_settings = settings_append (st_settings,
                     setting_new_int (get_setting_name (SETT_WEB_DLG_WIDTH),
                                      (int64_t) i_w));
    st_settings = settings_append (st_settings,
                     setting_new_int (get_setting_name (SETT_WEB_DLG_HEIGHT),
                                      (int64_t) i_h));

    i_res = js_settings_check_update_file (st_settings, s_cfg_file);
    settings_free_all (st_settings);
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update thumbnail jpg quality.
 */
int
setts_update_thumb_quality (const char *s_cfg_file,
                            const int   i_jpgq)
{
    Setting  *st_settings;
    int       i_res = ERR_OK;

    st_settings = setting_new_int (get_setting_name (SETT_THUMB_QUALITY),
                                   i_jpgq);

    i_res = js_settings_check_update_file (st_settings, s_cfg_file);
    settings_free_all (st_settings);
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings in SettList are an update to settings
 *         stored in settings file.
 */
char *
setts_check_update (const char    *s_cfg_file,
                    const Setting *st_settings,
                    int           *i_err)
{
    return js_settings_check_for_update (st_settings, s_cfg_file, i_err);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update file with new data.
 */
int
setts_update_file (const char *s_cfg_file,
                   const char *s_buff)
{
    return js_settings_update_file (s_buff, s_cfg_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings are an update and update file with new data
 *         if they are.
 */
int
setts_check_update_file (const char    *s_cfg_file,
                         const Setting *st_settings)
{
    return js_settings_check_update_file (st_settings, s_cfg_file);
}
/*----------------------------------------------------------------------------*/
