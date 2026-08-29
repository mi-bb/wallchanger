/**
 * @file  setts.h
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
#ifndef SETTS_H
#define SETTS_H

#include "../config.h"
#include "setting.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Settings enum values
 */
enum e_setting_values {
    SETT_BG_CMD = 0,     /**< Wallpaper set command */
    SETT_LAST_USED_OPT,  /**< Select last used on start */
    SETT_LAST_USED_STR,  /**< Last used wallpaper path */
    SETT_LAST_USED_WM,   /**< Id of last used window manager */
    SETT_WIN_WIDTH,      /**< Window width */
    SETT_WIN_HEIGHT,     /**< Window height */
    SETT_RANDOM_OPT,     /**< Random wallpaper select */
    SETT_INTERVAL_VAL,   /**< Wallpaper change interval */
    SETT_TIME_ALIGN_OPT, /**< Time align */
    SETT_WALL_ARRAY,     /**< Array with wallpapers */
    SETT_WEB_DLG_WIDTH,  /**< Web dilaog width */
    SETT_WEB_DLG_HEIGHT, /**< Web dilaog height */
    SETT_THUMB_QUALITY,  /**< Thumbnail jpg quality */
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
 * @param[in] st_settings  List of Setting items
 * @return    List of Setting items, with defaults appended for any that
 *            were missing. Must be used, as it may differ from
 *            st_settings if the list was empty (e.g. NULL).
 */
Setting *    setts_check_defaults (Setting    *st_settings);
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
 * @return    Status of data update
 *
 * @fn  int setts_update_last_used_wm (const char *s_cfg_file,
 *                                     const char *s_last_used_wm)
 *
 * @brief  Update last used window manager value in config file.
 *
 * @param[in] s_cfg_file      Config file path
 * @param[in] s_last_used_wm  Last used window manager name
 * @return    Status of data update
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
 * @return    Status of data update
 *
 * @fn  int setts_update_web_dlg_size(const char *s_cfg_file,
 *                                    const int   i_w,
 *                                    const int   i_h)
 *
 * @brief  Update wallpaper from web dialog size in config file.
 *
 * @param[in] s_cfg_file  Config file path
 * @param[in] i_w         Window width value
 * @param[in] i_h         Window height value
 * @return    Status of data update
 */
/*----------------------------------------------------------------------------*/
int setts_update_last_used     (const char *s_cfg_file,
                                const char *s_last_used);

int setts_update_last_used_wm  (const char *s_cfg_file,
                                const char *s_last_used_wm);

int setts_update_window_size   (const char *s_cfg_file,
                                const int   i_w,
                                const int   i_h);

int setts_update_web_dlg_size  (const char *s_cfg_file,
                                const int   i_w,
                                const int   i_h);

int setts_update_thumb_quality (const char *s_cfg_file,
                                const int   i_jpgq);

/*----------------------------------------------------------------------------*/
/**
 * @fn  char * setts_check_update (const char    *s_cfg_file,
 *                                 const Setting *st_settings,
 *                                 int           *i_err)
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
 * @fn  int setts_check_update_file (const char    *s_cfg_file,
 *                                   const Setting *st_settings)
 *
 * @brief  Check if settings are an update and update file with new data
 *         if they are.
 *
 * @param[in] s_cfg_file   Config file path
 * @param[in] st_settings  List of Setting items
 * @return    Saving file status, ERR_OK or error code
 */
/*----------------------------------------------------------------------------*/
char * setts_check_update      (const char    *s_cfg_file,
                                const Setting *st_settings,
                                int           *i_err)
                                __attribute__ ((nonnull (1)));

int    setts_update_file       (const char    *s_cfg_file,
                                const char    *s_buff)
                                __attribute__ ((nonnull (1, 2)));

int    setts_check_update_file (const char    *s_cfg_file,
                                const Setting *st_settings)
                                __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
#endif

