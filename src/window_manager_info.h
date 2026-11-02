/**
 * @file  window_manager_info.h
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
 * @brief  Window manager info getting functions.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef WINDOW_MANAGER_INFO_H
#define WINDOW_MANAGER_INFO_H

#include "setting_item.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of Xfce displays possible to set wallpaper.
 *
 * @return Null terminated list of display strings.
 */
char   ** wms_get_xfce_display_list  (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free null terminated list of strings.
 *
 * @param[in,out]  list  Null terminated list of strings
 * @return         none
 */
void      wms_free_xfce_display_list (char      **list);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create Xfce wallpaper set command with display name given in disp.
 *
 * @param[in] disp  String with display for command
 * @return    New string with wallpaper set command. After use it should be
 *            freed using free.
 */
char    * wms_get_xfce_command       (const char *disp);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get window manager info from user's local config file.
 *
 * @param[out] err  Error output
 * @return     List of Setting items with window manager info
 */
Setting * wms_get_wm_info_home       (int        *err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get window manager info from default config file.
 *
 * @param[out] err  Error output
 * @return     List of Setting items with window manager info
 */
Setting * wms_get_wm_info_data       (int        *err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get window manager info from user's config file and if it is not
 *         present, get data from default config file.
 *
 * @param[out] err  Error output
 * @return     List of Setting items with window manager info
 */
Setting * wms_get_wm_info            (int        *err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find window manager that is currently in use.
 *
 * @param[in]  wmlist  List of Setting items
 *
 * @return     Setting item with info about window manager, pointing into
 *             wmlist. It must not be freed separately - it stays owned by
 *             wmlist.
 */
const Setting * wms_get_current_wm   (const Setting *wmlist);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update wallpaper set command in window manager info config file.
 *
 * @param[in] wm_name  Name of window manager
 * @param[in] command  New command to set wallpaper
 * @return    Saving file status, ERR_OK or error code
 */
int       wms_update_wm_command      (const char *wm_name,
                                      const char *command);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Compare previously used window manager with present one, set
 *         wallpaper change command.
 *
 * @param[in]  cfg_file  Config file path
 * @param[in]  settings  List of Setting items
 * @param[in]  wmlist    Window manager info list
 * @param[out] err       Error output
 * @return     String with wallpaper set command. After use it should be freed
 *             using free
 */
char    * wms_get_wallpaper_command  (const char *cfg_file,
                                      Setting    *settings,
                                      Setting    *wmlist,
                                      int        *err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Compare user's wm info with default app wm info.
 *
 * Compare user's wm info with default app wm info, update user's config
 * file if there was some new data in default wm info (usually after adding
 * some new window managers in new version).
 *
 * @return  Error result
 */
/*----------------------------------------------------------------------------*/
int       wms_check_for_new_wms      (void);
/*----------------------------------------------------------------------------*/
#endif

