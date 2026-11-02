/**
 * @file  settings.c
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
#include "json_functions.h"
#include "setting_item.h"
#include "settings.h"
#include "errors.h"
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
get_setting_name (const int val)
{
    const char *res = nullptr;

    switch (val) {
        case SETT_BG_CMD:
            res = "Background set command";
            break;

        case SETT_LAST_USED_OPT:
            res = "Set last used wallpaper";
            break;
        
        case SETT_LAST_USED_STR:
            res = "Last used wallpaper file";
            break;

        case SETT_LAST_USED_WM:
            res = "Last used window manager";
            break;

        case SETT_WIN_WIDTH:
            res = "Window width";
            break;

        case SETT_WIN_HEIGHT:
            res = "Window height";
            break;

        case SETT_RANDOM_OPT:
            res = "Random wallpaper";
            break;

        case SETT_INTERVAL_VAL:
            res = "Wallpaper change interval";
            break;

        case SETT_TIME_ALIGN_OPT:
            res = "Time align";
            break;

        case SETT_WALL_ARRAY:
            res = "Backgrounds";
            break;

        case SETT_WEB_DLG_WIDTH:
            res = "Web dialog width";
            break;

        case SETT_WEB_DLG_HEIGHT:
            res = "Web dialog height";
            break;

        case SETT_THUMB_QUALITY:
            res = "Thumbnail quality";
            break;

        default:
            break;
    }
    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read program settings. 
 */
Setting *
setts_read (const char *cfg_file,
            int        *err)
{
    return json_settings_read (cfg_file, err);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check for a Setting with the name from data and insert a new
 *         one with the values from data if it is missing.
 *
 * @param[in] settings  List of Setting items
 * @param[in] data      Data with setting info to examine
 * @return    List of Setting items, with new one appended if it was
 *            missing. Must be used, as it may differ from settings
 *            if the list was empty.
 */
static Setting *
setts_check_setting (Setting           *settings,
                     const SettingData *data)
{
    const char *name = nullptr;
    Setting    *sett;

    name = get_setting_name (data->setting_id);

    if ((sett = settings_find (settings, name)) == nullptr) {
#ifdef DEBUG
        printf ("%s", name);
        printf (" not present, setting default ");
#endif
        if (data->setting_type == SET_VAL_INT) {
#ifdef DEBUG
            printf ("%" PRId64 "\n", data->default_int);
#endif
            settings = settings_append (settings,
                    setting_new_int (name, data->default_int));
        }
        else if (data->setting_type == SET_VAL_DOUBLE) {
#ifdef DEBUG
            printf ("%f\n", data->default_double);
#endif
            settings = settings_append (settings,
                    setting_new_double (name, data->default_double));
        }
        else if (data->setting_type == SET_VAL_STRING) {
#ifdef DEBUG
            printf ("%s\n", data->default_string);
#endif
            settings = settings_append (settings,
                    setting_new_string (name, data->default_string));
        }
        else if (data->setting_type == SET_VAL_ARRAY) {
#ifdef DEBUG
            printf ("\n");
#endif
            settings = settings_append (settings,
                          setting_new_array (name));
        }
    }
    else {
#ifdef DEBUG
        printf ("%s", name);
        printf (" OK\n");
#endif
    }

    return settings;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check Setting list values and set default ones if needed.
 */
Setting *
setts_check_defaults (Setting *settings)
{
    SettingData *data;

    SettingData defaults[] = {
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

    for (data = defaults; data->setting_id != -1; ++data) {
        settings = setts_check_setting (settings, data);
    }

    return settings;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update last used wallpaper position in config file. 
 */
int
setts_update_last_used (const char *cfg_file,
                        const char *last_used)
{
    Setting  *settings;
    int       res = ERR_OK;

    settings = setting_new_string (get_setting_name (SETT_LAST_USED_STR),
                                      last_used);

    res = json_settings_check_update_file (settings, cfg_file);
    settings_free_all (settings);
    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update last used window manager value in config file. 
 */
int
setts_update_last_used_wm (const char *cfg_file,
                           const char *last_used_wm)
{
    Setting  *settings;
    int       res = ERR_OK;

    settings = setting_new_string (get_setting_name (SETT_LAST_USED_WM),
                                      last_used_wm);

    res = json_settings_check_update_file (settings, cfg_file);
    settings_free_all (settings);
    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update window size in config file. 
 */
int
setts_update_window_size (const char *cfg_file,
                          const int   w,
                          const int   h)
{
    Setting  *settings = nullptr;
    int       res = ERR_OK;

    settings = settings_append (settings,
                     setting_new_int (get_setting_name (SETT_WIN_WIDTH),
                                      (int64_t) w));
    settings = settings_append (settings,
                     setting_new_int (get_setting_name (SETT_WIN_HEIGHT),
                                      (int64_t) h));

    res = json_settings_check_update_file (settings, cfg_file);
    settings_free_all (settings);
    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update wallpaper from web dialog size in config file.
 */
int
setts_update_web_dlg_size (const char *cfg_file,
                           const int   w,
                           const int   h)
{
    Setting  *settings = nullptr;
    int       res = ERR_OK;

    settings = settings_append (settings,
                     setting_new_int (get_setting_name (SETT_WEB_DLG_WIDTH),
                                      (int64_t) w));
    settings = settings_append (settings,
                     setting_new_int (get_setting_name (SETT_WEB_DLG_HEIGHT),
                                      (int64_t) h));

    res = json_settings_check_update_file (settings, cfg_file);
    settings_free_all (settings);
    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update thumbnail jpg quality.
 */
int
setts_update_thumb_quality (const char *cfg_file,
                            const int   jpgq)
{
    Setting  *settings;
    int       res = ERR_OK;

    settings = setting_new_int (get_setting_name (SETT_THUMB_QUALITY),
                                   jpgq);

    res = json_settings_check_update_file (settings, cfg_file);
    settings_free_all (settings);
    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings in SettList are an update to settings
 *         stored in settings file.
 */
char *
setts_check_update (const char    *cfg_file,
                    const Setting *settings,
                    int           *err)
{
    return json_settings_check_for_update (settings, cfg_file, err);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update file with new data.
 */
int
setts_update_file (const char *cfg_file,
                   const char *buff)
{
    return json_settings_update_file (buff, cfg_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings are an update and update file with new data
 *         if they are.
 */
int
setts_check_update_file (const char    *cfg_file,
                         const Setting *settings)
{
    return json_settings_check_update_file (settings, cfg_file);
}
/*----------------------------------------------------------------------------*/
