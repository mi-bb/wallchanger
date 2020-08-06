/**
 * @file  setts.h
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
 * @brief  Settings manage functions
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef SETTS_H
#define SETTS_H

#include "setting.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Settings enum values
 */
enum {
    SETTING_BG_CMD = 0,     /**< Wallpaper set command */
    SETTING_LAST_USED_OPT,  /**< Select last used on start */
    SETTING_LAST_USED_STR,  /**< Last used wallpaper path */
    SETTING_LAST_USED_WM,   /**< Id of last used window manager */
    SETTING_WIN_WIDTH,      /**< Window width */
    SETTING_WIN_HEIGHT,     /**< Window height */
    SETTING_RANDOM_OPT,     /**< Random wallpaper select */
    SETTING_INTERVAL_VAL,   /**< Wallpaper change interval */
    SETTING_TIME_ALIGN_OPT, /**< Time align */
    SETTING_WALL_ARRAY      /**< Array with wallpapers */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get name of setting in config file, based on enum value.
 *
 * @param[in]  i_val  Setting enum value
 * @return     String with setting name
 */
const char * get_setting_name     (const int   i_val) __attribute__ ((const));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check Setting list values and set default ones if needed.
 *
 * @param[in,out] st_settings  List of Setting items
 * @return        none
 */
void         setts_check_defaults (Setting    *st_settings);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read program settings from a given config file.
 *
 * @param[in]  s_cfg_file  Config file path
 * @param[out] i_err       Error output
 * @return     Setting list with settings or null pointer
 */
Setting *    setts_read           (const char *s_cfg_file,
                                   int        *i_err)
                                   __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
/**
 * @fn  int setts_update_last_used (const char *s_cfg_file,
 *                                  const char *s_last_used)
 *
 * @brief  Update last used wallpaper file name in config file. 
 *
 * @param[in] s_cfg_file   Config file path
 * @param[in] s_last_used  Last used file path
 * @return    Updating data in settings file status
 *
 * @fn  int setts_update_last_used_wm (const char *s_cfg_file,
 *                                     const int   i_last_used_wm)
 *
 * @brief  Update last used window manager value in config file. 
 *
 * @param[in] s_cfg_file      Config file path
 * @param[in] i_last_used_wm  Last used window manager
 * @return    Updating data in settings file status
 *
 * @fn  int setts_update_window_size (const char *s_cfg_file,
 *                                    const int   i_w,
 *                                    const int   i_h)
 *
 * @brief  Update window size in config file. 
 *
 * @param[in] s_cfg_file  Config file path
 * @param[in] i_w         Window width value
 * @param[in] i_h         Window height value
 * @return    Updating data in settings file status
 */
/*----------------------------------------------------------------------------*/
int setts_update_last_used    (const char *s_cfg_file,
                               const char *s_last_used);

int setts_update_last_used_wm (const char *s_cfg_file,
                               const char *s_last_used_wm);

int setts_update_window_size  (const char *s_cfg_file,
                               const int   i_w,
                               const int   i_h);
/*----------------------------------------------------------------------------*/
/**
 * @fn  char * setts_check_update (const char *s_cfg_file,
 *                                 Setting    *st_settings,
 *                                 int        *i_err)
 *
 * @brief  Check if settings in SettList are an update to settings
 *         stored in settings file.
 *
 * @param[in]  s_cfg_file   Config file path
 * @param[in]  st_settings  List of Setting items
 * @param[out] i_err        Error output
 * @return     String with updated settings or null if there is no update.
 *
 * @fn  int setts_update_file (const char *s_cfg_file,
 *                             const char *s_buff)
 *
 * @brief  Update file with new data.
 *
 * @param[in] s_cfg_file  Config file path
 * @param[in] s_buff      String with data to save
 * @return    Saving file status, ERR_OK or error code
 *
 * @fn  int setts_check_update_file (const char *s_cfg_file,
 *                                   Setting    *st_settings)
 *
 * @brief  Check if settings are an update and update file with new data
 *         if they are.
 *
 * @param[in] s_cfg_file   Config file path
 * @param[in] st_settings  List of Setting items
 * @return    Saving file status, ERR_OK or error code
 */
/*----------------------------------------------------------------------------*/
char * setts_check_update      (const char *s_cfg_file,
                                Setting    *st_settings,
                                int        *i_err)
                                __attribute__ ((nonnull (1)));

int    setts_update_file       (const char *s_cfg_file,
                                const char *s_buff)
                                __attribute__ ((nonnull (1, 2)));

int    setts_check_update_file (const char *s_cfg_file,
                                Setting    *st_settings)
                                __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
#endif

