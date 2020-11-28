/**
 * @file  wmsfn.h
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
 * @brief  Wallpaper set command functions.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef WMSFN_H
#define WMSFN_H

#include "setting.h"
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
 * @param[in,out]  s_list  Null terminated list of strings
 * @return         none
 */
void      wms_free_xfce_display_list (char      **s_list);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create Xfce wallpaper set command with display name given in s_disp.
 *
 * @param[in] s_disp  String with display for command
 * @return    New string with wallpaper set command. After use it should be
 *            freeed using free.
 */
char    * wms_get_xfce_command       (const char *s_disp);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get window manager info from user's local config file.
 *
 * @param[out] i_err  Error output
 * @return     List of Setting items with window manager info
 */
Setting * wms_get_wm_info_home       (int        *i_err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get window manager info from default config file.
 *
 * @param[out] i_err  Error output
 * @return     List of Setting items with window manager info
 */
Setting * wms_get_wm_info_data       (int        *i_err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get window manager info from user's config file and if it is not
 *         present, get data from default config file.
 *
 * @param[out] i_err  Error output
 * @return     List of Setting items with window manager info
 */
Setting * wms_get_wm_info            (int        *i_err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find window manager that is currently in use.
 *
 * @param[in]  st_wmsl  List of Setting items
 *
 * return     Setting item with info about window manager.
 *            After use it should be freed using free.
 */
const Setting * wms_get_current_wm   (const Setting *st_wmsl);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update wallpaper set command in window manager info config file.
 *
 * @param[in] s_wm_name  Name of window manager
 * @param[in] s_command  New command to set wallpaper
 * @return    Saving file status, ERR_OK or error code
 */
int       wms_update_wm_command      (const char *s_wm_name,
                                      const char *s_command);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Compare previously used window manager with present one, set
 *         wallpaper change command.
 *
 * @param[in]  s_cfg_file   Config file path
 * @param[in]  st_settings  List of Setting items
 * @param[in]  st_wmlist    Window manager info list
 * @param[out] i_err        Error output
 * @return     String with wallpaper set command. After use it should be freed
 *             using free
 */
char    * wms_get_wallpaper_command  (const char *s_cfg_file,
                                      Setting    *st_settings,
                                      Setting    *st_wmlist,
                                      int        *i_err);
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

