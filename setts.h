/**
 * @file  setts.h
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
#ifndef SETTS_H
#define SETTS_H

#include "wallsett.h"
#include "settlist.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Settings enum values
 */
enum {
    SETTING_BG_CMD,
    SETTING_LAST_USED_OPT,
    SETTING_LAST_USED_STR,
    SETTING_LAST_USED_POS,
    SETTING_WIN_WIDTH,
    SETTING_WIN_HEIGHT,
    SETTING_RANDOM_OPT,
    SETTING_INTERVAL_VAL,
    SETTING_WALL_ARRAY
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get name of setting in config file, based on enum value.
 *
 * @param[in]  i_val  Setting enum value
 * @return     String with setting name
 */
const char * get_setting_name (int i_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read program settings. 
 *
 * @param[in]  s_fname  Name of file with settings
 * @param[out] i_err    Error output
 * @return     Settlist list of Setting objects or null pointer
 */
SettList * settings_read (const char *s_fname,
                          int        *i_err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert settings in SettList format to WallSett format.
 *
 * @param[in]  st_list  Settings in SettList
 * @param[out] ws_sett  WallSet settings output
 * @return     none
 */
void settlist_to_wallset (SettList *st_list,
                          WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int settings_update_last_used (const char *s_last_used,
 *                                     const char *s_fname)
 * @brief     Update last used wallpaper position in config file. 
 * @param[in] s_last_used  Last used file path
 * @param[in] s_fname      Name of file with settings
 * @return    Updating data in settings file status
 *
 * @fn  int settings_update_window_size (const int   i_w,
 *                                       const int   i_h,
 *                                       const char *s_cfg_file)
 * @brief     Update window size in config file. 
 * @param[in] i_w         Window width value
 * @param[in] i_h         Window height value
 * @param[in] s_cfg_file  Config file path
 * @return    Updating data in settings file status
 */
int settings_update_last_used   (const char *s_last_used,
                                 const char *s_fname);
/*----------------------------------------------------------------------------*/
int settings_update_window_size (const int   i_w,
                                 const int   i_h,
                                 const char *s_cfg_file);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int settings_check_update (SettList    *st_list,
 *                                 const char  *s_fname,
 *                                 char       **s_out_buff)
 * @brief      Check if settings in SettList are an update to settings
 *             stored in settings file.
 * @param[in]  st_list     List of settings
 * @param[in]  s_fname     Config file name
 * @param[out] s_out_buff  Buffer to write data
 * @return     ERR_OK or error code
 *
 * @fn  int settings_update_file (const char *s_buff,
 *                                const char *s_fname)
 * @brief     Update file with new data.
 * @param[in] s_buff   String with data to save
 * @param[in] s_fname  File name to save data
 * @return    Saving file status, ERR_OK or error code
 *
 * @fn  int settings_check_update_file (SettList   *st_list,
 *                                      const char *s_fname)
 * @brief     Check if settings are an update and update file with new data
 *            if they are.
 * @param[in] st_list  List of settings
 * @param[in] s_fname  File name to save data
 * @return    Saving file status, ERR_OK or error code
 */
/*----------------------------------------------------------------------------*/
int settings_check_update      (SettList    *st_list,
                                const char  *s_fname,
                                char       **s_out_buff);
/*----------------------------------------------------------------------------*/
int settings_update_file       (const char  *s_buff,
                                const char  *s_fname);
/*----------------------------------------------------------------------------*/
int settings_check_update_file (SettList    *st_list,
                                const char  *s_fname);
/*----------------------------------------------------------------------------*/
#endif

